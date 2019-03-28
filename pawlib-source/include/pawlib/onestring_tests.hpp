/** Tests for OneString [PawLIB]
  * Version: 0.4
  *
  * Author(s): Jason C. McDonald, Scott Taylor, Bo Volwiler
  */

/* LICENSE
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

#ifndef PAWLIB_ONESTRING_TESTS_HPP
#define PAWLIB_ONESTRING_TESTS_HPP

#include <string>

#include "pawlib/goldilocks.hpp"
#include "pawlib/onestring.hpp"

using namespace pawlib::ioformat;
using namespace pawlib;

namespace pawlib
{
    class TestOneString : public Test
    {
        public:
            enum TestStringType {
                CHAR,
                OCHAR_ASCII,
                OCHAR_UNICODE,
                CSTR_ASCII,
                CSTR_UNICODE,
                STR_ASCII,
                STR_UNICODE,
                OSTR_ASCII,
                OSTR_UNICODE
            };

            char ch_1 = 'M';
            char ch_2 = 'm';

            OneChar ochr_ascii_1 = "M";
            OneChar ochr_ascii_2 = "m";

            OneChar ochr_unicode_1 = "🐭";
            OneChar ochr_unicode_2 = "🦊";

            OneString ostr_ch_ascii_1 = "M";
            OneString ostr_ch_ascii_2 = "m";
            OneString ostr_ch_unicode_1 = "🐭";
            OneString ostr_ch_unicode_2 = "🦊";

            // We'll extract the c-strings from these as needed.
            std::string str_ascii_1 = "The quick brown fox jumped over the lazy dog.";
            std::string str_unicode_1 = "The quick brown 🦊 jumped over the lazy 🐶.";
            std::string str_ascii_2 = "Jackdaws love my big sphinx of quartz.";
            std::string str_unicode_2 = "🐦 ❤️ my big sphinx of 💎.";

            OneString ostr_ascii_1 = "The quick brown fox jumped over the lazy dog.";
            OneString ostr_unicode_1 = "The quick brown 🦊 jumped over the lazy 🐶.";
            OneString ostr_ascii_2 = "Jackdaws love my big sphinx of quartz.";
            OneString ostr_unicode_2 = "🐦 ❤️ my big sphinx of 💎.";

        protected:
            TestStringType stringType;

            testdoc_t title;

        public:
            explicit TestOneString(TestStringType type)
            :stringType(type)
            {
                switch(stringType)
                {
                    case CHAR:
                    {
                        title = "(char)";
                        break;
                    }
                    case OCHAR_ASCII:
                    {
                        title = "(OneChar, ASCII)";
                        break;
                    }
                    case OCHAR_UNICODE:
                    {
                        title = "(OneChar, Unicode)";
                        break;
                    }
                    case CSTR_ASCII:
                    {
                        title = "(c-string, ASCII)";
                        break;
                    }
                    case CSTR_UNICODE:
                    {
                        title = "(c-string, Unicode)";
                        break;
                    }
                    case STR_ASCII:
                    {
                        title = "(std::string, ASCII)";
                        break;
                    }
                    case STR_UNICODE:
                    {
                        title = "(std::string, Unicode)";
                        break;
                    }
                    case OSTR_ASCII:
                    {
                        title = "(OneString, ASCII)";
                        break;
                    }
                    case OSTR_UNICODE:
                    {
                        title = "(OneString, Unicode)";
                        break;
                    }
                }
            }

            virtual testdoc_t get_title() override = 0;

            virtual testdoc_t get_docs() override = 0;
    };

    // P-tB4001[a-i]
    class TestOneString_Assign : public TestOneString
    {
        protected:
            OneString test;

        public:
            explicit TestOneString_Assign(TestStringType type)
            :TestOneString(type)
            {}

            testdoc_t get_title() override
            {
                return "OneString: Assign " + title;
            }

            testdoc_t get_docs() override
            {
                return "Test assignment to a OneString.";
            }

            bool janitor() override {
                OneString test = "";
                return true;
            }

            bool run() override {
                switch(stringType)
                {
                    case CHAR:
                    {
                        test.assign(ch_1);
                        PL_ASSERT_EQUAL(test, ostr_ch_ascii_1);
                        return true;
                    }
                    case OCHAR_ASCII:
                    {
                        test.assign(ochr_ascii_1);
                        PL_ASSERT_EQUAL(test, ostr_ch_ascii_1);
                        return true;
                    }
                    case OCHAR_UNICODE:
                    {
                        test.assign(ochr_unicode_1);
                        PL_ASSERT_EQUAL(test, ostr_ch_unicode_1);
                        return true;
                    }
                    case CSTR_ASCII:
                    {
                        test.assign(str_ascii_1.c_str());
                        PL_ASSERT_EQUAL(test, ostr_ascii_1);
                        return true;
                    }
                    case CSTR_UNICODE:
                    {
                        test.assign(str_unicode_1.c_str());
                        PL_ASSERT_EQUAL(test, ostr_unicode_1);
                        return true;
                    }
                    case STR_ASCII:
                    {
                        test.assign(str_ascii_1);
                        PL_ASSERT_EQUAL(test, ostr_ascii_1);
                        return true;
                    }
                    case STR_UNICODE:
                    {
                        test.assign(str_unicode_1);
                        PL_ASSERT_EQUAL(test, ostr_unicode_1);
                        return true;
                    }
                    case OSTR_ASCII:
                    {
                        test.assign(ostr_ascii_1);
                        PL_ASSERT_EQUAL(test, ostr_ascii_1);
                        return true;
                    }
                    case OSTR_UNICODE:
                    {
                        test.assign(ostr_unicode_1);
                        PL_ASSERT_EQUAL(test, ostr_unicode_1);
                        return true;
                    }
                    default:
                    {
                        // Can't reach
                        return false;
                    }
                }
            }
    };

    // P-tB4002[a-i]
    class TestOneString_Equals : public TestOneString
    {
        protected:
            OneString test;

        public:
            explicit TestOneString_Equals(TestStringType type)
            :TestOneString(type)
            {}

            testdoc_t get_title() override
            {
                return "OneString: Equals " + title;
            }

            testdoc_t get_docs() override
            {
                return "Test comparison with the equals() function.";
            }

            bool janitor() override {
                OneString test = "";
                return true;
            }

            bool run() override {
                switch(stringType)
                {
                    case CHAR:
                    {
                        test = ch_1;
                        PL_ASSERT_TRUE(test.equals(ch_1));
                        PL_ASSERT_FALSE(test.equals(ch_2));
                        return true;
                    }
                    case OCHAR_ASCII:
                    {
                        test = ochr_ascii_1;
                        PL_ASSERT_TRUE(test.equals(ochr_ascii_1));
                        PL_ASSERT_FALSE(test.equals(ochr_ascii_2));
                        return true;
                    }
                    case OCHAR_UNICODE:
                    {
                        test = ochr_unicode_1;
                        PL_ASSERT_TRUE(test.equals(ochr_unicode_1));
                        PL_ASSERT_FALSE(test.equals(ochr_unicode_2));
                        return true;
                    }
                    case CSTR_ASCII:
                    {
                        test = str_ascii_1;
                        PL_ASSERT_TRUE(test.equals(str_ascii_1.c_str()));
                        PL_ASSERT_FALSE(test.equals(str_ascii_2.c_str()));
                        return true;
                    }
                    case CSTR_UNICODE:
                    {
                        test = str_unicode_1;
                        PL_ASSERT_TRUE(test.equals(str_unicode_1.c_str()));
                        PL_ASSERT_FALSE(test.equals(str_unicode_2.c_str()));
                        return true;
                    }
                    case STR_ASCII:
                    {
                        test = str_ascii_1;
                        PL_ASSERT_TRUE(test.equals(str_ascii_1));
                        PL_ASSERT_FALSE(test.equals(str_ascii_2));
                        return true;
                    }
                    case STR_UNICODE:
                    {
                        test = str_unicode_1;
                        PL_ASSERT_TRUE(test.equals(str_unicode_1));
                        PL_ASSERT_FALSE(test.equals(str_unicode_2));
                        return true;
                    }
                    case OSTR_ASCII:
                    {
                        test = ostr_ascii_1;
                        PL_ASSERT_TRUE(test.equals(ostr_ascii_1));
                        PL_ASSERT_FALSE(test.equals(ostr_ascii_2));
                        return true;
                    }
                    case OSTR_UNICODE:
                    {
                        test = ostr_unicode_1;
                        PL_ASSERT_TRUE(test.equals(ostr_unicode_1));
                        PL_ASSERT_FALSE(test.equals(ostr_unicode_2));
                        return true;
                    }
                    default:
                    {
                        // Can't reach
                        return false;
                    }
                }
            }
    };

    // P-tB4003[a-i]
    class TestOneString_OpEquals : public TestOneString
    {
        protected:
            OneString test;

        public:
            explicit TestOneString_OpEquals(TestStringType type)
            :TestOneString(type)
            {}

            testdoc_t get_title() override
            {
                return "OneString: Equals (==) " + title;
            }

            testdoc_t get_docs() override
            {
                return "Test comparison with the == operator.";
            }

            bool janitor() override {
                OneString test = "";
                return true;
            }

            bool run() override {
                switch(stringType)
                {
                    case CHAR:
                    {
                        test = ch_1;
                        PL_ASSERT_TRUE(test == ch_1);
                        PL_ASSERT_FALSE(test == ch_2);
                        return true;
                    }
                    case OCHAR_ASCII:
                    {
                        test = ochr_ascii_1;
                        PL_ASSERT_TRUE(test == ochr_ascii_1);
                        PL_ASSERT_FALSE(test == ochr_ascii_2);
                        return true;
                    }
                    case OCHAR_UNICODE:
                    {
                        test = ochr_unicode_1;
                        PL_ASSERT_TRUE(test == ochr_unicode_1);
                        PL_ASSERT_FALSE(test == ochr_unicode_2);
                        return true;
                    }
                    case CSTR_ASCII:
                    {
                        test = str_ascii_1;
                        PL_ASSERT_TRUE(test == str_ascii_1.c_str());
                        PL_ASSERT_FALSE(test == str_ascii_2.c_str());
                        return true;
                    }
                    case CSTR_UNICODE:
                    {
                        test = str_unicode_1;
                        PL_ASSERT_TRUE(test == str_unicode_1.c_str());
                        PL_ASSERT_FALSE(test == str_unicode_2.c_str());
                        return true;
                    }
                    case STR_ASCII:
                    {
                        test = str_ascii_1;
                        PL_ASSERT_TRUE(test == str_ascii_1);
                        PL_ASSERT_FALSE(test == str_ascii_2);
                        return true;
                    }
                    case STR_UNICODE:
                    {
                        test = str_unicode_1;
                        PL_ASSERT_TRUE(test == str_unicode_1);
                        PL_ASSERT_FALSE(test == str_unicode_2);
                        return true;
                    }
                    case OSTR_ASCII:
                    {
                        test = ostr_ascii_1;
                        PL_ASSERT_TRUE(test == ostr_ascii_1);
                        PL_ASSERT_FALSE(test == ostr_ascii_2);
                        return true;
                    }
                    case OSTR_UNICODE:
                    {
                        test = ostr_unicode_1;
                        PL_ASSERT_TRUE(test == ostr_unicode_1);
                        PL_ASSERT_FALSE(test == ostr_unicode_2);
                        return true;
                    }
                    default:
                    {
                        // Can't reach
                        return false;
                    }
                }
            }
    };

    // P-tB4004[a-i]
    class TestOneString_OpNotEquals : public TestOneString
    {
        protected:
            OneString test;

        public:
            explicit TestOneString_OpNotEquals(TestStringType type)
            :TestOneString(type)
            {}

            testdoc_t get_title() override
            {
                return "OneString: Not Equals (!=) " + title;
            }

            testdoc_t get_docs() override
            {
                return "Test comparison with the != operator.";
            }

            bool janitor() override {
                OneString test = "";
                return true;
            }

            bool run() override {
                switch(stringType)
                {
                    case CHAR:
                    {
                        test = ch_1;
                        PL_ASSERT_TRUE(test != ch_2);
                        PL_ASSERT_FALSE(test != ch_1);
                        return true;
                    }
                    case OCHAR_ASCII:
                    {
                        test = ochr_ascii_1;
                        PL_ASSERT_TRUE(test != ochr_ascii_2);
                        PL_ASSERT_FALSE(test != ochr_ascii_1);
                        return true;
                    }
                    case OCHAR_UNICODE:
                    {
                        test = ochr_unicode_1;
                        PL_ASSERT_TRUE(test != ochr_unicode_2);
                        PL_ASSERT_FALSE(test != ochr_unicode_1);
                        return true;
                    }
                    case CSTR_ASCII:
                    {
                        test = str_ascii_1;
                        PL_ASSERT_TRUE(test != str_ascii_2.c_str());
                        PL_ASSERT_FALSE(test != str_ascii_1.c_str());
                        return true;
                    }
                    case CSTR_UNICODE:
                    {
                        test = str_unicode_1;
                        PL_ASSERT_TRUE(test != str_unicode_2.c_str());
                        PL_ASSERT_FALSE(test != str_unicode_1.c_str());
                        return true;
                    }
                    case STR_ASCII:
                    {
                        test = str_ascii_1;
                        PL_ASSERT_TRUE(test != str_ascii_2);
                        PL_ASSERT_FALSE(test != str_ascii_1);
                        return true;
                    }
                    case STR_UNICODE:
                    {
                        test = str_unicode_1;
                        PL_ASSERT_TRUE(test != str_unicode_2);
                        PL_ASSERT_FALSE(test != str_unicode_1);
                        return true;
                    }
                    case OSTR_ASCII:
                    {
                        test = ostr_ascii_1;
                        PL_ASSERT_TRUE(test != ostr_ascii_2);
                        PL_ASSERT_FALSE(test != ostr_ascii_1);
                        return true;
                    }
                    case OSTR_UNICODE:
                    {
                        test = ostr_unicode_1;
                        PL_ASSERT_TRUE(test != ostr_unicode_2);
                        PL_ASSERT_FALSE(test != ostr_unicode_1);
                        return true;
                    }
                    default:
                    {
                        // Can't reach
                        return false;
                    }
                }
            }
    };

    ///////////// REUSABLE /////////////

    class TestOneString_PopBack : public Test
    {
        protected:
            OneString start = "The quick brown 🦊 jumped over the lazy 🐶📣";
            OneString target = "The quick brown 🦊 jumped over the lazy 🐶";
            OneString test;

        public:
            TestOneString_PopBack(){}

            testdoc_t get_title() override
            {
                return "OneString: pop_back()";
            }

            testdoc_t get_docs() override
            {
                return "Tests for removing characters from a OneString using pop_back().";
            }

            bool pre() override
            {
                return janitor();
            }

            bool janitor() override
            {
                test = start;
                return (test == start);
            }

            bool run() override
            {
                test.pop_back();
                PL_ASSERT_EQUAL(test, target);
                return true;
            }
    };

    class TestOneString_Length : public Test
    {
        protected:
            OneString start = "Ø÷Ø÷Ø÷Ø÷Ø÷Ø÷Ø÷";
            OneString test;

        public:
            TestOneString_Length(){}

            testdoc_t get_title() override
            {
                return "OneString: length()";
            }

            testdoc_t get_docs() override
            {
                return "Test the length() function.";
            }

            bool pre() override
            {
                return janitor();
            }

            bool janitor() override
            {
                test = start;
                return (test == start);
            }

            bool run() override
            {
                PL_ASSERT_EQUAL(test.length(), 14U);
                test.pop_back();
                PL_ASSERT_EQUAL(test.length(), 13U);
                return true;
            }
    };

    class TestOneString_Empty : public Test
    {
        protected:
            OneString test1 = "";
            OneString test2 = "a";

        public:
            TestOneString_Empty(){}

            testdoc_t get_title() override
            {
                return "OneString: empty()";
            }

            testdoc_t get_docs() override
            {
                return "Test that the empty() function returns true on an empty string.";
            }

            bool run() override
            {
                PL_ASSERT_TRUE(test1.empty());
                PL_ASSERT_FALSE(test2.empty());
                return true;
            }
    };

    class TestOneString_Clear : public Test
    {
        protected:
            OneString start = "Ø÷Ø÷Ø÷Ø÷Ø÷Ø÷Ø÷";
            OneString test;

        public:
            TestOneString_Clear(){}

            testdoc_t get_title() override
            {
                return "OneString: clear()";
            }

            testdoc_t get_docs() override
            {
                return "Tests for clearing all items in a OneString with clear().";
            }

            bool pre() override {
                return janitor();
            }

            bool janitor() override
            {
                test = start;
                return (!test.empty());
            }

            bool run() override
            {
                test.clear();
                PL_ASSERT_TRUE(test.empty());
                return true;
            }
    };

    class TestOneString_Swap : public Test
    {
        protected:
            OneString beforeOne;
            OneString beforeTwo;
            OneString afterOne;
            OneString afterTwo;

        public:
            TestOneString_Swap(){}

            testdoc_t get_title() override
            {
                return "OneString: swap()";
            }

            testdoc_t get_docs() override
            {
                return "Test swapping two OneStrings.";
            }

            bool pre() override
            {
                return janitor();
            }

            bool janitor() override
            {
                beforeOne = "Pre Swap Ø";
                beforeTwo = "Ø Post Swap";
                afterOne = beforeOne;
                afterTwo = beforeTwo;
                return true;
            }

            bool run() override
            {
                beforeOne.swap(beforeTwo);
                PL_ASSERT_EQUAL(beforeOne, afterTwo);
                PL_ASSERT_EQUAL(beforeTwo, afterOne);
                return true;
            }
    };

    class TestOneString_Substr : public Test
    {
        protected:
            OneString start = "⛰ The Matterhorn ⛰";

        public:
            TestOneString_Substr(){}

            testdoc_t get_title() override
            {
                return "OneString: substr()";
            }

            testdoc_t get_docs() override
            {
                return "Test creating a substring from a OneString.";
            }

            bool run() override
            {
                OneString partial = start.substr(12,4);
                PL_ASSERT_EQUAL(partial, "horn");
                return true;
            }
    };

    class TestOneString_At : public Test
    {
        protected:
            OneString before = "⛰ The Matterhorn ⛰";

        public:
            TestOneString_At(){}

            testdoc_t get_title() override
            {
                return "OneString: at()";
            }

            testdoc_t get_docs() override
            {
                return "Test accessing one character of a OneString with at()";
            }

            bool run() override
            {
                OneChar toCheck;
                toCheck = "⛰";
                PL_ASSERT_EQUAL(before.at(17), toCheck);
                return true;
            }
    };

    class TestOneString_ForceResize : public Test
    {
        protected:
            OneString target = "🐉🐉🐉🐉🐉🐉🐉🐉🐉🐉🐉🐉🐉🐉🐉🐉🐉🐉🐉🐉🐉🐉";
        public:
            TestOneString_ForceResize(){}

            testdoc_t get_title() override
            {
                return "OneString: Force Resize";
            }

            testdoc_t get_docs() override
            {
                return "Append characters to force resizing of the OneString's internal structure.";
            }

            bool run() override
            {
                OneString test;
                for (size_t i = 0; i < target.length(); ++i)
                {
                    test.append("🐉");
                }
                PL_ASSERT_EQUAL(test, target);
                return true;
            }
    };

    class TestSuite_OneString : public TestSuite
    {
        public:
            explicit TestSuite_OneString(){}

            void load_tests() override;

            testdoc_t get_title() override
            {
                return "PawLIB: OneString Tests";
            }
    };
}
#endif // PAWLIB_ONESTRING_TESTS_HPP
