/** IOChannel [PawLIB]
  * Version: 2.0.0
  *
  * Allows managed, custom output to any console or terminal.
  * See WHAT IS IOCHANNEL? below.
  *
  * Author: Jason C. McDonald
  */

/* LICENSE (BSD-3-Clause)
 * Copyright (c) 2016-2019 MousePaw Media.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 * CONTRIBUTING
 * See https://www.mousepawmedia.com/developers for information
 * on how to contribute to our projects.
 */

/* WHAT IS IOCHANNEL?
 * IOChannel is intended both as a replacement and wrapper for `std::iostream` and
 * `stdio.h/printf`. It allows for messages and errors to be output to multiple
 * sources simultaneously and asynchronously using signals. New
 * messages from any source are added to the back of the queue, and arbitrary
 * outputs can read them asynchronously from the front, either destructively or
 * non-destructively.
 *
 * Each output is also able to individually timing, as well as which messages it is
 * interested in and how it reads them, without interfering with the behavior
 * of other outputs.
 *
 * EXTERNAL OUTPUTS
 * An external output waits for a signal to be dispatched before it collects its
 * messages. Different signals are dispatched for different levels of verbosity
 * and categories.
 *
 * INTERNAL OUTPUTS
 * Optionally, iochannel can output to the terminal automatically via either
 * `std::iostream` or `stdio.h/printf`. This output can be controlled externally.
 * For example, a developer might choose to create pseudocommands in their
 * command-line that allow them to change verbosity on-the-fly while the program
 * is running.
 *
 * VERBOSITY
 * The concept of verbosity allows for developers to write and leave all manner
 * of useful output data, including debug information, detailed error messages,
 * and outright snark. Verbosity can be toggled globally for a channel,
 * or on a connection-by-connection basis.
 *
 * Verbosity ranges from 0 (only essential messages) to 3 (literally all messages).
 *
 * CATEGORY
 * Messages can be assigned a category, which makes it easier to different messages
 * to be sent to different outputs, or otherwise be handled differently. At the
 * moment, the categories are...
 * - Normal Messages
 * - Warnings
 * - Errors
 * - Debug Output
 *
 * CROSS-PLATFORM FORMATTING
 * IOChannel offers coloring and basic formatting on both UNIX and Windows systems
 * via the same interface.
 */

#ifndef PAWLIB_IOCHANNEL_HPP
#define PAWLIB_IOCHANNEL_HPP

//Needed for output sources.
#include <stdio.h>
#include <iostream>

//Needed for the `intptr_t` type
#include <cstdint>

//Needed for `ceil()`
#include <cmath>

//Bitset
#include <bitset>

//Needed for checking types.
#include <typeinfo>

//Needed for handling passed-in exceptions.
#include <exception>

//Signals and callbacks.
#include "eventpp/callbacklist.h"

/*We are only using std::string and std::queue temporarily.
These need to be swapped out for pawlib alternatives ASAP.*/
#include <string>

//We use C's classes often.
#include <cstdio>

#include "pawlib/core_types.hpp"
#include "pawlib/stdutils.hpp"

#include "pawlib/onechar.hpp"
#include "pawlib/onestring.hpp"

class pure_tril;
class tril;
class iochannel;

template<typename T>
T flags_and(const T& lhs, const T& rhs)
{
    return static_cast<T>
        (static_cast<int>(lhs) & static_cast<int>(rhs));
}

template<typename T>
T flags_or(const T& lhs, const T& rhs)
{
    return static_cast<T>
        (static_cast<int>(lhs) | static_cast<int>(rhs));
}

template<typename T>
T flags_xor(const T& lhs, const T& rhs)
{
    return static_cast<T>
        (static_cast<int>(lhs) ^ static_cast<int>(rhs));
}

template<typename T>
T flags_twiddle(const T& rhs)
{
    return static_cast<T>(~static_cast<int>(rhs));
}

/** The category of the message. */
enum class IOCat
{
    /// No category. Internal use only; no correlating signal.
    none = 0,
    /// The default value - anything that doesn't fit elsewhere.
    normal = 1,
    /// Warnings, but not necessarily errors.
    warning = 2,
    /// Error messages.
    error = 4,
    /// Debug messages, such as variable outputs.
    debug = 8,
    /// Testing messages that we may want shut off during benchmarking.
    testing = 16,
    /// All message categories. Internal use only; no correlating signal.
    all = 31
};

inline IOCat operator&(const IOCat& lhs, const IOCat& rhs)
{
    return flags_and(lhs, rhs);
}

inline IOCat operator|(const IOCat& lhs, const IOCat& rhs)
{
    return flags_or(lhs, rhs);
}

inline IOCat operator^(const IOCat& lhs, const IOCat& rhs)
{
    return flags_xor(lhs, rhs);
}

inline IOCat operator~(const IOCat& lhs)
{
    return flags_twiddle(lhs);
}


/** Controls the output of the IOChannel. */
enum class IOCtrl
{
    /// Send, keep formatting
    send = 1,
    clear = 2,
    r = 4,
    n = 8,
    flush = 16,
    /// Send with carriage return (\r), keep formatting
    sendc = 1 | 4 | 16,
    /// Send with line feed (\n), keep formatting
    sendl = 1 | 8 | 16,
    /// Send, clear formatting
    end = 1 | 2,
    /// End with carriage return (\r), clear formatting
    endc = 1 | 2 | 4 | 16,
    /// End with line feed (\n), clear formatting
    endl = 1 | 2 | 8 | 16,
};

inline IOCtrl operator&(const IOCtrl& lhs, const IOCtrl& rhs)
{
    return flags_and(lhs, rhs);
}

inline IOCtrl operator|(const IOCtrl& lhs, const IOCtrl& rhs)
{
    return flags_or(lhs, rhs);
}

inline IOCtrl operator^(const IOCtrl& lhs, const IOCtrl& rhs)
{
    return flags_xor(lhs, rhs);
}

inline IOCtrl operator~(const IOCtrl& lhs)
{
    return flags_twiddle(lhs);
}

/** Basic cursor movement. */
enum class IOCursor
{
    // Move cursor left ('\[1D]')
    left,
    // Move cursor right ('\[1C]')
    right
};

enum class IOEchoMode
{
    /** Don't automatically output messages via the stdout echo. */
    none = 0,
    /** Output messages to stdout via C-style `printf`. */
    printf = 1,
    /** Output messages to stdout via C++-style `std::cout`. */
    cout = 2
};

enum class IOFormatBase
{
    bin = 2,
    b2 = 2,
    ter = 3,
    b3 = 3,
    quat = 4,
    b4 = 4,
    quin = 5,
    b5 = 5,
    sen = 6,
    b6 = 6,
    sep = 7,
    b7 = 7,
    oct = 8,
    b8 = 8,
    b9 = 9,
    dec = 10,
    b10 = 10,
    und = 11,
    b11 = 11,
    duo = 12,
    doz = 12,
    b12 = 12,
    tri = 13,
    b13 = 13,
    tetra = 14,
    b14 = 14,
    pent = 15,
    b15 = 15,
    hex = 16,
    b16 = 16,
    b17 = 17,
    b18 = 18,
    b19 = 19,
    vig = 20,
    b20 = 20,
    b21 = 21,
    b22 = 22,
    b23 = 23,
    b24 = 24,
    b25 = 25,
    b26 = 26,
    b27 = 27,
    b28 = 28,
    b29 = 29,
    b30 = 30,
    b31 = 31,
    b32 = 32,
    b33 = 33,
    b34 = 34,
    b35 = 35,
    b36 = 36
};

enum class IOFormatBool
{
    lower = 0,
    upper = 1,
    caps = 2,
    numeral = 3,
    scott = 4
};

enum class IOFormatCharValue
{
    as_char = 0,
    as_int = 1
};

enum class IOFormatMemSep
{
    ///Output as one long string.
    none = 0,
    ///Output with spaces between bytes.
    byte = (1 << 0),
    ///Output with bars between words (8 bytes).
    word = (1 << 1),
    ///Output with spaces between bytes and bars between words.
    all = 3
};

inline IOFormatMemSep operator&(const IOFormatMemSep& lhs, const IOFormatMemSep& rhs)
{
    return flags_and(lhs, rhs);
}

inline IOFormatMemSep operator|(const IOFormatMemSep& lhs, const IOFormatMemSep& rhs)
{
    return flags_or(lhs, rhs);
}

inline IOFormatMemSep operator^(const IOFormatMemSep& lhs, const IOFormatMemSep& rhs)
{
    return flags_xor(lhs, rhs);
}

inline IOFormatMemSep operator~(const IOFormatMemSep& lhs)
{
    return flags_twiddle(lhs);
}

enum class IOFormatNumCase
{
    ///Print all letter digits as lowercase.
    lower = 0,
    ///Print all letter digits as uppercase.
    upper = 1
};

enum class IOFormatPtr
{
    ///Print the value at the address.
    value = 0,
    ///Print the actual memory address.
    address = 1,
    ///Dump the hexadecimal representation of the memory at address.
    memory = 2
};

enum class IOFormatSciNotation
{
    ///Turn off all scientific notation.
    none = 0,
    ///Automatically select the best option.
    automatic = 1,
    ///Turn on all scientific notation.
    on = 2
};

struct IOFormatSignificands
{
    explicit IOFormatSignificands(unsigned int s):significands(s){}
    int significands = 14;
};

/**The standard ANSI text attributes.*/
enum class IOFormatTextAttr
{
    ///Turn of all attributes.
    none = 0,
    ///Bold text.
    bold = 1,
    ///Underlined text.
    underline = 4,
    ///Inverted text colors, also known as "reverse video".
    invert = 6
};
// TODO: Change `IOFormatTextAttr` to a bitfield, to allow multiple format flags.

/** The standard ANSI text background colors. */
enum class IOFormatTextBG
{
    //None.
    none = 0,
    ///Black text background.
    black = 40,
    ///Red text background.
    red = 41,
    ///Green text background.
    green = 42,
    ///Yellow text background.
    yellow = 43,
    ///Blue text background.
    blue = 44,
    ///Magenta text background.
    magenta = 45,
    ///Cyan text background.
    cyan = 46,
    ///White text background.
    white = 47
};

/** The standard ANSI text foreground colors. */
enum class IOFormatTextFG
{
    //None.
    none = 0,
    ///Black text.
    black = 30,
    ///Red text.
    red = 31,
    ///Green text
    green = 32,
    ///Yellow text.
    yellow = 33,
    ///Blue text.
    blue = 34,
    ///Magenta text.
    magenta = 35,
    ///Cyan text.
    cyan = 36,
    ///White text.
    white = 37
};

/**Indicate how many bytes to read from any pointer that isn't
 * recognized explicitly by iochannel, including void pointers.
 * This will not override the memory dump read size of built-in types.*/
struct IOMemReadSize
{
    /**Indicate how many bytes to read from any pointer that isn't
     * recognized explicitly by iochannel, including void pointers.
     * This will not override the memory dump read size of built-in
     * types.
     * CAUTION: Misuse can cause SEGFAULT or other memory errors.
     * \param the number of bytes to read*/
    explicit IOMemReadSize(unsigned int i):readsize(i){}
    unsigned int readsize = 1;

    void operator=(unsigned int rhs)
    {
        readsize = rhs;
    }
};

/** The level of verbosity necessary for the message to display. */
enum class IOVrb
{
    /**Only essential messages and errors. For normal end-use.
    Shipping default.*/
    quiet = 0,
    /**Common messages and errors. For common and normal end-user
    testing.*/
    normal = 1,
    /**Most messages and errors. For detailed testing and
    debugging.*/
    chatty = 2,
    /**Absolutely everything. For intense testing, detailed
    debugging, and driving the developers crazy.*/
    tmi = 3
};

class IOFormat
{
    friend iochannel;

    private:
        IOFormatBase fmt_base;
        IOFormatBool fmt_bool;
        IOFormatCharValue fmt_char_value;
        IOFormatMemSep fmt_mem_sep;
        IOFormatNumCase fmt_numeral_case;
        IOFormatPtr fmt_ptr;
        IOFormatSciNotation fmt_sci_notation;
        IOFormatSignificands fmt_significands;
        IOFormatTextAttr fmt_text_attr;
        IOFormatTextBG fmt_text_bg;
        IOFormatTextFG fmt_text_fg;

    public:
        IOFormat()
        : fmt_base(IOFormatBase::b10),
            fmt_bool(IOFormatBool::lower),
            fmt_char_value(IOFormatCharValue::as_char),
            fmt_mem_sep(IOFormatMemSep::all),
            fmt_numeral_case(IOFormatNumCase::upper),
            fmt_ptr(IOFormatPtr::value),
            fmt_sci_notation(IOFormatSciNotation::automatic),
            fmt_significands(IOFormatSignificands(14)),
            fmt_text_attr(IOFormatTextAttr::none),
            fmt_text_bg(IOFormatTextBG::none),
            fmt_text_fg(IOFormatTextFG::none)
            {}

        IOFormat(const IOFormat& cpy)
        : fmt_base(cpy.fmt_base),
            fmt_bool(cpy.fmt_bool),
            fmt_char_value(cpy.fmt_char_value),
            fmt_mem_sep(cpy.fmt_mem_sep),
            fmt_numeral_case(cpy.fmt_numeral_case),
            fmt_ptr(cpy.fmt_ptr),
            fmt_sci_notation(cpy.fmt_sci_notation),
            fmt_significands(cpy.fmt_significands),
            fmt_text_attr(cpy.fmt_text_attr),
            fmt_text_bg(cpy.fmt_text_bg),
            fmt_text_fg(cpy.fmt_text_fg)
            {}

        IOFormat& operator=(const IOFormat& cpy)
        {
            fmt_base = cpy.fmt_base;
            fmt_bool = cpy.fmt_bool;
            fmt_char_value = cpy.fmt_char_value;
            fmt_mem_sep = cpy.fmt_mem_sep;
            fmt_numeral_case = cpy.fmt_numeral_case;
            fmt_ptr = cpy.fmt_ptr;
            fmt_sci_notation = cpy.fmt_sci_notation;
            fmt_significands = cpy.fmt_significands;
            fmt_text_attr = cpy.fmt_text_attr;
            fmt_text_bg = cpy.fmt_text_bg;
            fmt_text_fg = cpy.fmt_text_fg;
            return *this;
        }

        IOFormat& operator<<(const IOFormatBase& rhs){fmt_base = rhs; return *this;}
        IOFormat& operator<<(const IOFormatBool& rhs){fmt_bool = rhs; return *this;}
        IOFormat& operator<<(const IOFormatCharValue& rhs){fmt_char_value = rhs; return *this;}
        IOFormat& operator<<(const IOFormatMemSep& rhs){fmt_mem_sep = rhs; return *this;}
        IOFormat& operator<<(const IOFormatNumCase& rhs){fmt_numeral_case = rhs; return *this;}
        IOFormat& operator<<(const IOFormatPtr& rhs){fmt_ptr = rhs; return *this;}
        IOFormat& operator<<(const IOFormatSciNotation& rhs){fmt_sci_notation = rhs; return *this;}
        IOFormat& operator<<(const IOFormatSignificands& rhs){fmt_significands = rhs; return *this;}
        IOFormat& operator<<(const IOFormatTextAttr& rhs){fmt_text_attr = rhs; return *this;}
        IOFormat& operator<<(const IOFormatTextBG& rhs){fmt_text_bg = rhs; return *this;}
        IOFormat& operator<<(const IOFormatTextFG& rhs){fmt_text_fg = rhs; return *this;}


};

/**An iochannel allows console output to be custom routed to one or more
* text-based output channels, including the terminal. It supports various
* advanced functions, formatting and colors, and message priority.*/
class iochannel
{
    public:
        /**Declares a new iochannel instance.*/
        iochannel();

        /** Eventpp signal (callback list) for categories. */
        typedef eventpp::CallbackList<void (std::string,
            IOCat)> IOSignalCat;

        /** Eventpp signal (callback list) for verbosities. */
        typedef eventpp::CallbackList<void (std::string,
            IOVrb)> IOSignalVrb;

        /** Eventpp signal (callback list) for everything,
             * transmitting the message, the verbosity, and the category. */
        typedef eventpp::CallbackList<void (std::string,
            IOVrb, IOCat)> IOSignalFull;

        /** Eventpp signal (callback list) for everything,
             * transmitting only the message. */
        typedef eventpp::CallbackList<void (std::string)> IOSignalAll;


        /* NOTE: In the examples below, the verbosity-related signals must
            * transmit what category the message is (since verbosity is
            * inherent and assumed). The inverse is true of category-related
            * signals. */

        /** Emitted when a message with verbosity 0 (quiet) is broadcast.
             * Callback must be of form 'void callback(string,
             * IOCat){}'
             */
        IOSignalCat signal_v_quiet;

        /** Emitted when a message with verbosity <= 1 (normal) is broadcast.
             * Callback must be of form 'void callback(string,
             * IOCat){}'
             */
        IOSignalCat signal_v_normal;

        /** Emitted when a message with verbosity <=2 (chatty) is broadcast.
             * Callback must be of form 'void callback(string,
             * IOCat){}'
             */
        IOSignalCat signal_v_chatty;

        /** Emitted when a message with verbosity <=3 (tmi) is broadcast.
             * Callback must be of form 'void callback(string,
             * IOCat){}'
             */
        IOSignalCat signal_v_tmi;

        /** Emitted when a message with category "normal" is broadcast.
             * Callback must be of form 'void callback(string,
             * IOVrb){}'
             */
        IOSignalVrb signal_c_normal;

        /** Emitted when a message with category "warning" is broadcast.
             * Callback must be of form 'void callback(string,
             * IOVrb){}'
             */
        IOSignalVrb signal_c_warning;

        /** Emitted when a message with category "error" is broadcast.
             * Callback must be of form 'void callback(string,
             * IOVrb){}'
             */
        IOSignalVrb signal_c_error;

        /** Emitted when a message with category "debug" is broadcast.
             * Callback must be of form 'void callback(string,
             * IOVrb){}'
             */
        IOSignalVrb signal_c_debug;

        /** Emitted when a message with category "testing" is broadcast.
             * Callback must be of form 'void callback(string,
             * IOVrb){}'
             */
        IOSignalVrb signal_c_testing;

        /** Emitted when any message is broadcast.
             * Callback must be of form 'void callback(string,
             * IOVrb, IOCat){}'
             */
        IOSignalFull signal_full;

        /** Emitted when any message is broadcast, but only transmits the
             * message, and omits the verbosity and category.
             * Callback must be of form 'void callback(string){}'
             */
        IOSignalAll signal_all;

        // Overloaded stream insertion operators for IOChannel.

        //These need custom implementations.
        iochannel& operator<<(const bool&);
        iochannel& operator<<(const tril&);
        iochannel& operator<<(const char&);
        iochannel& operator<<(const std::string&);
        iochannel& operator<<(const onechar&);
        iochannel& operator<<(const onestring&);

        inline iochannel& operator<<(const unsigned char& rhs){return resolve_integer(rhs);}
        inline iochannel& operator<<(const int& rhs){return resolve_integer(rhs);}
        inline iochannel& operator<<(const unsigned int& rhs){return resolve_integer(rhs);}
        inline iochannel& operator<<(const short int& rhs){return resolve_integer(rhs);}
        inline iochannel& operator<<(const unsigned short int& rhs){return resolve_integer(rhs);}
        inline iochannel& operator<<(const long int& rhs){return resolve_integer(rhs);}
        inline iochannel& operator<<(const unsigned long int& rhs){return resolve_integer(rhs);}
        inline iochannel& operator<<(const long long int& rhs){return resolve_integer(rhs);}
        inline iochannel& operator<<(const unsigned long long int& rhs){return resolve_integer(rhs);}

        inline iochannel& operator<<(const float& rhs){return resolve_float(rhs);}
        inline iochannel& operator<<(const double& rhs){return resolve_float(rhs);}
        inline iochannel& operator<<(const long double& rhs){return resolve_float(rhs);}

        template <typename T>
        inline iochannel& operator<<(const T* rhs){return resolve_pointer_smart(rhs);}

        inline iochannel& operator<<(const void* rhs){return resolve_pointer(rhs);}
        inline iochannel& operator<<(const bool* rhs){return resolve_pointer(rhs);}
        inline iochannel& operator<<(const char* rhs){return resolve_pointer(rhs);}
        inline iochannel& operator<<(const unsigned char* rhs){return resolve_pointer(rhs);}
        inline iochannel& operator<<(const int* rhs){return resolve_pointer(rhs);}
        inline iochannel& operator<<(const unsigned int* rhs){return resolve_pointer(rhs);}
        inline iochannel& operator<<(const short int* rhs){return resolve_pointer(rhs);}
        inline iochannel& operator<<(const unsigned short int* rhs){return resolve_pointer(rhs);}
        inline iochannel& operator<<(const long int* rhs){return resolve_pointer(rhs);}
        inline iochannel& operator<<(const unsigned long int* rhs){return resolve_pointer(rhs);}
        inline iochannel& operator<<(const long long int* rhs){return resolve_pointer(rhs);}
        inline iochannel& operator<<(const unsigned long long int* rhs){return resolve_pointer(rhs);}
        inline iochannel& operator<<(const float* rhs){return resolve_pointer(rhs);}
        inline iochannel& operator<<(const double* rhs){return resolve_pointer(rhs);}
        inline iochannel& operator<<(const long double* rhs){return resolve_pointer(rhs);}
        inline iochannel& operator<<(const std::string* rhs){return resolve_pointer(rhs);}
        inline iochannel& operator<<(const onechar* rhs){return resolve_pointer(rhs);}
        inline iochannel& operator<<(const onestring* rhs){return resolve_pointer(rhs);}

        iochannel& operator<<(const std::exception& rhs);

        //All of these need custom implementations.
        template<const long unsigned int T>
        iochannel& operator<<(const std::bitset<T>& rhs)
        {
            // Store the old values for the two flags we use.
            IOMemReadSize old_readsize = readsize;
            IOFormatPtr old_ptr = fmt.fmt_ptr;

            /* The readsize (in bytes) is the bitset size (bits) divided
                * by 8 and rounded to the nearest integer. */
            readsize = static_cast<unsigned int>(ceil((T/8)));
            // We want a memory dump.
            fmt << IOFormatPtr::memory;

            // Remove the const-ness from the pointer.
            std::bitset<T>* rhs_ptr = const_cast<std::bitset<T>*>(&rhs);
            /* Convert to a void pointer for easier resolution.
                * We must store the result as a pointer for returning shortly.
                */
            iochannel* r = &resolve_pointer(static_cast<void*>(rhs_ptr));

            // Restore our prior flag values.
            fmt << old_ptr;
            readsize = old_readsize;

            /* Return the value at the pointer, which will be implicitly
                * converted to reference by the function return. */
            return *r;
        }

        iochannel& operator<<(const IOFormatBase& rhs) {process_fmt(rhs); return *this;}
        iochannel& operator<<(const IOFormatBool& rhs) {process_fmt(rhs); return *this;}
        iochannel& operator<<(const IOFormatCharValue& rhs) {process_fmt(rhs); return *this;}
        iochannel& operator<<(const IOFormatMemSep& rhs) {process_fmt(rhs); return *this;}
        iochannel& operator<<(const IOFormatNumCase& rhs) {process_fmt(rhs); return *this;}
        iochannel& operator<<(const IOFormatPtr& rhs) {process_fmt(rhs); return *this;}
        iochannel& operator<<(const IOFormatSciNotation& rhs) {process_fmt(rhs); return *this;}
        iochannel& operator<<(const IOFormatSignificands& rhs) {process_fmt(rhs); return *this;}

        iochannel& operator<<(const IOFormatTextAttr& rhs)
        {
            if (process_fmt(rhs))
            {
                dirty_attributes = true;
            }
            return *this;
        }

        iochannel& operator<<(const IOFormatTextBG& rhs)
        {
            if (process_fmt(rhs))
            {
                dirty_attributes = true;
            }
            return *this;
        }

        iochannel& operator<<(const IOFormatTextFG& rhs)
        {
            if (process_fmt(rhs))
            {
                dirty_attributes = true;
            }
            return *this;
        }

        iochannel& operator<<(const IOFormat&);
        iochannel& operator<<(const IOCursor&);
        iochannel& operator<<(const IOMemReadSize&);
        iochannel& operator<<(const IOVrb&);
        iochannel& operator<<(const IOCat&);
        iochannel& operator<<(const IOCtrl&);

        void configure_echo(IOEchoMode, IOVrb = IOVrb::tmi, IOCat = IOCat::all);

        /** Suppress a category from broadcasting at all.
             * \param the category to suppress
             */
        void shutup(IOCat);

        /** Suppress verbosities higher than the one given.
             * This is also mainly used to reset verbosity back to "normal"
             * (the default).
             * \param the maximum verbosity to permit (default=IOVrb::normal)
             */
        void shutup(IOVrb = IOVrb::normal);

        /** Permit a category to broadcast.
             * \param the category to broadcast
             */
        void speakup(IOCat);

        /** Permit a verbosity (and all below it) to broadcast.
             * Functionally identical to `shutup(IOVrb)`.
             * \param the maximum verbosity to permit
             */
        void speakup(IOVrb);

        /** Permit all messages (full verbosity and all categories)
             * to be broadcast.
             */
        void speakup();

        ~iochannel();
    protected:
        std::string msg;

        // Which categories are permitted.
        IOCat process_cat = IOCat::all;
        // The maximum verbosity to permit.
        IOVrb process_vrb = IOVrb::tmi;

        // Which method should be used for IOChannel's default stdout echo?
        IOEchoMode echo_mode = IOEchoMode::printf;
        // The category to echo.
        IOCat echo_cat = IOCat::all;
        // The maximum verbosity to echo.
        IOVrb echo_vrb = IOVrb::tmi;

        /* MESSAGE ATTRIBUTES
            * These are set by enum and flags,
            * and should be reset after each message (unless
            * the user ends the message with an EoM stream
            * control with a "keep" flag.
            */

        IOFormat fmt;

        // How many bytes of data to read from a pointer.
        IOMemReadSize readsize = IOMemReadSize(1);

        /* Message verbosity. */
        IOVrb vrb = IOVrb::normal;
        /* Message category. */
        IOCat cat = IOCat::normal;
        /* Message parsable */
        tril parse = maybe;

        template<typename T>
        bool process_fmt(T val)
        {
            //If we cannot parse because of `shutup()` settings, abort.
            if(!can_parse()){return false;}

            fmt << val;
            return true;
        }

        template <typename T>
        iochannel& resolve_pointer_smart(const T* rhs)
        {
            //If we cannot parse because of `shutup()` settings, abort.
            if(!can_parse()){return *this;}

            switch(fmt.fmt_ptr)
            {
                //If we are to print as value...
                case IOFormatPtr::value:
                {
                    *this << "[iochannel cannot interpret value at pointer of this type.]";
                    break;
                }
                //If we are to print as address...
                case IOFormatPtr::address:
                {
                    //Inject raw address, overriding with byte read size 1.
                    inject(rhs, 1, false);
                    break;
                }
                //If we are to print the raw memory...
                case IOFormatPtr::memory:
                {
                    //Inject raw address with the object size as read_size.
                    inject(rhs, sizeof(T), true);
                    break;
                }
            }
            return *this;
        }

        /** Resolves any common pointer. This can handle
             * all atomic and numeric C and C++ types.
             * \param the pointer to resolve
             */
        template <typename T>
        iochannel& resolve_pointer(const T*);

        /** Resolves a void pointer, which allows
             * for raw data dumps from any pointer
             * imaginable.
             * \param the pointer to resolve
             */
        iochannel& resolve_pointer(const void*);

        /** Resolves a char pointer.
             * \param the pointer to resolve
             */
        iochannel& resolve_pointer(const char*);

        /** Resolves any integer.
             * \param the integer variable to resolve
             */
        template <typename T>
        iochannel& resolve_integer(const T&);

        /** Resolves any floating point number variable.
             * \param the floating point variable to resolve
             */
        template <typename T>
        iochannel& resolve_float(const T&);

        //The string containing the format.
        std::string format = "";

        /**Flush the standard output.*/
        void flush();

        /**Move the cursor given the command.
             * \param the cursor command
             */
        void move_cursor(const IOCursor&);

        /**Insert a single character without need for null terminator.
        * \param the character to insert
        */
        void inject(char);

        /**Insert a C string into the output stream. Automatically applies
        * unapplied attributes before inserting text.
        * \param the C-string to insert.
        * \param whether the call was recursive. (Internal use only!)
        */
        void inject(const char*, bool=false);

        /**Insert a memory address or its raw contents into the output
         * stream.
         * \param the address to insert
         * \param the size of the object referenced
         * \param whether to print literal address (false) or memory
         * dump (true); default false*/
        void inject(const void*, unsigned int len, bool=false);

        /**Transmit the current pending output stream and reset in
        * preparation for the next message.
        * \param whether to retain the flags
        */
        void transmit(bool=false);

        ///Dirty flag raised when attributes are changed and not yet applied.
        bool dirty_attributes = false;

        /**Apply formatting attributes (usually ANSI) that are pending.
        * \param true if new attributes were applied
        */
        bool apply_attributes();

        /**Returns whether the vrb and cat match parsing (shutup) rules.*/
        bool can_parse();

        /**Clear the channel's message substring array.*/
        void clear_msg();

        /**Reset all attributes.*/
        void reset_attributes();

        /**Reset all flags.*/
        void reset_flags();

};

///Global external (static) instance of iochannel.
extern iochannel ioc;

#endif // PAWLIB_IOCHANNEL_HPP
