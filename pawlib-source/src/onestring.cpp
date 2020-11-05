#include "pawlib/onestring.hpp"

/*******************************************
* Constructors + Destructor
*******************************************/
onestring::onestring()
:_capacity(BASE_SIZE), _elements(0), internal(nullptr), _c_str(0)
{
    allocate(this->_capacity);
    //assign('\0');
}

onestring::onestring(char ch)
:_capacity(BASE_SIZE), _elements(0), internal(nullptr), _c_str(0)
{
    allocate(this->_capacity);
    assign(ch);
}

onestring::onestring(const onechar& ochr)
:_capacity(BASE_SIZE), _elements(0), internal(nullptr), _c_str(0)
{
    allocate(this->_capacity);
    assign(ochr);
}

onestring::onestring(const char* cstr)
:_capacity(BASE_SIZE), _elements(0), internal(nullptr), _c_str(0)
{
    allocate(this->_capacity);
    assign(cstr);
}

onestring::onestring(const std::string& str)
:_capacity(BASE_SIZE), _elements(0), internal(nullptr), _c_str(0)
{
    allocate(this->_capacity);
    append(str);
}

onestring::onestring(const onestring& ostr)
:_capacity(BASE_SIZE), _elements(0), internal(nullptr), _c_str(0)
{
    allocate(this->_capacity);
    assign(ostr);
}

onestring::~onestring()
{
    if (_c_str != nullptr)
    {
        delete[] _c_str;
    }

    if (internal != nullptr)
    {
        delete[] internal;
    }
}

/*******************************************
* Memory Management
*******************************************/

void onestring::allocate(size_t capacity)
{
    this->_capacity = capacity;

    // If we're allocating down, throw away the excess elements.
    if (this->_elements > this->_capacity)
    {
        this->_elements = this->_capacity;
    }

    // Allocate a new array with the new size.
    onechar* newArr = new onechar[this->_capacity];

    // If an old array exists...
    if(this->internal != nullptr)
    {
        for (size_t i = 0; i < this->_elements; ++i)
        {
            newArr[i] = std::move(this->internal[i]);
        }

        // Delete the old structure
        delete[] internal;
        this->internal = nullptr;
    }

    // Store the new structure.
    this->internal = newArr;
}

void onestring::expand(size_t expansion)
{
    reserve(this->_elements + expansion);
}

void onestring::reserve(size_t elements)
{
    // If we're already large enough, don't reallocate.
    if (this->_capacity >= elements) { return; }

    // A capacity of 0 will trigger a complete reallocation
    if (this->_capacity == 0)
    {
        this->_capacity = BASE_SIZE;
    }

    // If we're about to blow past indexing, manually set the capacity.
    if (elements >= RESIZE_LIMIT)
    {
        this->_capacity = npos;
    }

    // Expand until we have enough space.
    // cppcheck-suppress knownConditionTrueFalse
    while (this->_capacity < elements)
    {
        this->_capacity *= RESIZE_FACTOR;
    }

    allocate(this->_capacity);
}

void onestring::resize(size_t elements)
{
    // Don't reallocate if we already have the exact size needed.
    if (this->_capacity == elements) { return; }

    // Reallocate to EXACTLY the needed size.
    allocate(elements);
}

void onestring::resize(size_t elements, const onechar& ch)
{
    int to_add = elements - this->_elements;
    std::cout << to_add << std::endl;
    resize(elements);

    if (to_add > 0)
    {
        append(ch, to_add);
    }
}

void onestring::shift(size_t fromIndex, int offset)
{
    // WARNING: We do NOT handle expansion in this function; only shifting.

    // If we're shifting forward...
    if (offset > 0)
    {
        // Work from last element being moved to first
        size_t target = _elements;
        // Copy each element forward to its new position.
        while (target-- > fromIndex)
        {
            this->internal[target + offset] = this->internal[target];
        }
    }
    // Otherwise, if we're shifting backward...
    else if (offset < 0)
    {
        // Work from first element being moved to last
        size_t target = fromIndex;
        // Copy each element back to its new position.
        while (target < _elements)
        {
            this->internal[target + offset] = this->internal[target];
            ++target;
        }
    }
}

void onestring::shrink_to_fit()
{
    // We simply need to reallocate for the EXACT number of elements we have
    allocate(this->_elements);
}

/*******************************************
* Accessors
*******************************************/

onechar& onestring::at(size_t pos)
{
    if (pos > _elements)
    {
        throw std::out_of_range("Onestring::at(): Index out of bounds.");
    }

    return internal[pos];
}

const onechar& onestring::at(size_t pos) const
{
    if (pos > _elements)
    {
        throw std::out_of_range("Onestring::at(): Index out of bounds.");
    }

    return internal[pos];
}

onechar& onestring::back()
{
    // WARNING: If string is empty, this is undefined (but memory safe)
    // Return a reference to the last element in the string.
    return this->internal[_elements - 1];
}

const onechar& onestring::back() const
{
    // WARNING: If string is empty, this is undefined (but memory safe)
    // Return a reference to the last element in the string.
    return this->internal[_elements - 1];
}

size_t onestring::capacity() const
{
    return _capacity;
}

size_t onestring::copy(char* arr, size_t max, size_t len, size_t pos) const
{
    // Reminder: len and pos default to 0

    if (pos >= this->_elements)
    {
        throw std::out_of_range("Onestring::copy(): specified pos out of range");
    }

    // Remove one char from the max to account for the null terminator.
    --max;

    // The indices for the c-string and (this) onestring.
    size_t cstr_i = 0;
    size_t ostr_i = pos;

    // Account for unspecified len, or adjust so it doesn't blow past the end.
    if (len == 0 || len + pos > this->_elements)
    {
        len = this->_elements - pos;
    }

    while (cstr_i < max && ostr_i < (pos + len))
    {
        memcpy(arr + cstr_i, this->internal[ostr_i].internal, this->internal[ostr_i].size);
        cstr_i += this->internal[ostr_i].size;
        ++ostr_i;
    }

    arr[cstr_i] = '\0';

    return cstr_i;
}

const char* onestring::c_str() const
{
    // If we have a c-string instance cached, deallocate it.
    if (this->_c_str != nullptr)
    {
        delete[] this->_c_str;
    }

    // Allocate a new c-string.
    size_t n = size();
    this->_c_str = new char[n];

    // Convert and store each onechar's value in the c-string
    char* dest = this->_c_str;
    for(size_t i = 0; i < _elements; ++i)
    {
        memcpy(dest, internal[i].internal, internal[i].size);
        dest += internal[i].size;
    }

    // Append the null terminator to the end of the c-string
    this->_c_str[n-1] = '\0';

    // Return the pointer to the new cached c-string
    return this->_c_str;
}

bool onestring::empty() const
{
    return (_elements == 0);
}

onechar& onestring::front()
{
    // WARNING: If string is empty, this is undefined (but memory safe)
    // Return a reference to the first element in the string.
    return this->internal[0];
}

const onechar& onestring::front() const
{
    // WARNING: If string is empty, this is undefined (but memory safe)
    // Return a reference to the last element in the string.
    return this->internal[0];
}

size_t onestring::length() const
{
    return _elements;
}

onestring onestring::substr(size_t pos, size_t len) const
{
    if (pos >= this->_elements)
    {
        throw std::out_of_range("Onestring::substr(): specified pos out of range");
    }

    onestring r;
    // Calculate size of substr (number of elements)
    size_t elements_to_copy = (len > _elements - pos) ? (_elements - pos) : len;
    // Reserve necessary space in the new onestring
    r.reserve(elements_to_copy);
    // Copy the characters for the substring
    for (size_t i = 0; i < elements_to_copy; ++i)
    {
        r.internal[i] = this->internal[pos + i];
    }
    // Record how many elements were copied.
    r._elements = elements_to_copy;
    return r;
}

size_t onestring::size() const
{
    // Start counting at 1 to account for the null terminator.
    size_t bytes = 1;
    // Sum all of the byte lengths of the onechars.
    for(size_t i = 0; i < _elements; ++i)
    {
        bytes += this->internal[i].size;
    }
    return bytes;
}

size_t onestring::size(size_t len, size_t pos) const
{
    // Reminder: pos defaults to 0

    // Ensure the starting position is in range.
    if (pos >= this->_elements)
    {
        throw std::out_of_range("Onestring::size(): specified pos out of range");
    }

    // Start counting at 1 to account for the null terminator.
    size_t bytes = 1;

    // Sum all of the byte lengths of the onechars in the range
    for(size_t i = pos; i < (pos + len) && i < _elements; ++i)
    {
        bytes += this->internal[i].size;
    }
    return bytes;
}

/*******************************************
* Comparison
********************************************/

int onestring::compare(const char ch) const
{
    int sizeDiff = this->_elements - 1;
    if (sizeDiff == 0)
    {
        return this->internal[0].compare(ch);
    }
    return sizeDiff;
}

int onestring::compare(const onechar& ochr) const
{
    int sizeDiff = this->_elements - 1;
    if (sizeDiff == 0)
    {
        return this->internal[0].compare(ochr);
    }
    return sizeDiff;
}

int onestring::compare(const char* cstr) const
{
    /* This algorithm will not return a meaningful integer; only its
        * relation to 0 will be useful */

    int sizeDiff = this->_elements - characterCount(cstr);

    if (sizeDiff == 0)
    {
        size_t j = 0;
        // If the strings are the same length, compare each character...
        for(size_t i = 0; i < this->_elements; ++i)
        {
            // Return the compare result of the first character mismatch
            int r = this->internal[i].compare(cstr+j);
            if ( r != 0) { return r; }

            j += onechar::evaluateLength(cstr+j);
        }
    }
    return sizeDiff;
}

int onestring::compare(const std::string& str) const
{
    return compare(str.c_str());
}

int onestring::compare(const onestring& ostr) const
{
    /* This algorithm will not return a meaningful integer; only its
        * relation to 0 will be useful */

    int sizeDiff = this->_elements - ostr._elements;
    if (sizeDiff == 0)
    {
        // If the strings are the same length, compare each character...
        for(size_t i = 0; i < this->_elements; ++i)
        {
            // Return the compare result of the first character mismatch
            int r = this->internal[i].compare(ostr.internal[i]);
            if ( r != 0) { return r; }
        }
    }
    return sizeDiff;
}

bool onestring::equals(const char ch) const
{
    return (this->_elements == 1 && this->internal[0] == ch);
}

bool onestring::equals(const onechar& ochr) const
{
    return (this->_elements == 1 && this->internal[0] == ochr);
}

bool onestring::equals(const char* cstr) const
{
    size_t index = 0;
    size_t compare_to = 0;
    if (this->_elements != characterCount(cstr)) { return false; }
    while(cstr[index] != '\0')
    {
        if (!(this->internal[compare_to].equals_at(cstr + index))) { return false; }
        index += onechar::evaluateLength(cstr + index);
        if(compare_to++ > this->_elements) { return false; }
    }

    return true;
}

bool onestring::equals(const std::string& str) const
{
    return equals(str.c_str());
}

bool onestring::equals(const onestring& ostr) const
{
    if (this->_elements != ostr._elements) { return false; }

    for (size_t index = 0; index < this->_elements; ++index)
    {
        if (this->internal[index] != ostr.internal[index]) { return false; }
    }
    return true;
}

/*******************************************
* Mutators
********************************************/

onestring& onestring::append(const char ch, size_t repeat)
{
    expand(repeat);

    for(size_t a = 0; a < repeat; ++a)
    {
        // Insert a 1-byte ASCII char
        internal[_elements++] = ch;
    }
    return *this;
}

onestring& onestring::append(const onechar& ochr, size_t repeat)
{
    expand(repeat);

    for(size_t a = 0; a < repeat; ++a)
    {
        internal[_elements++] = ochr;
    }
    return *this;
}

onestring& onestring::append(const char* cstr, size_t repeat)
{
    size_t len = characterCount(cstr);
    expand(len * repeat);

    for(size_t a = 0; a < repeat; ++a)
    {
        size_t index = 0;
        // Loop through each character in the string literal
        while(cstr[index] != '\0')
        {
            index += internal[_elements++].parseFromString(cstr, index);
        }
    }
    return *this;
}

onestring& onestring::append(const std::string& str, size_t repeat)
{
    // Parse the internal c string directly.
    append(str.c_str(), repeat);
    return *this;
}

onestring& onestring::append(const onestring& ostr, size_t repeat)
{
    expand(ostr._elements * repeat);

    for(size_t a = 0; a < repeat; ++a)
    {
        for(size_t i = 0; i < ostr._elements; ++i)
        {
            this->internal[_elements + i] = ostr.internal[i];
        }

        _elements += ostr._elements;
    }

    return *this;
}

onestring& onestring::assign(const char ch)
{
    clear();
    reserve(1);
    this->internal[_elements++] = ch;
    return *this;
}

onestring& onestring::assign(const onechar& ochr)
{
    clear();
    reserve(1);
    this->internal[_elements++] = ochr;
    return *this;
}

onestring& onestring::assign(const char* cstr)
{
    clear();

    size_t len = characterCount(cstr);
    reserve(len);

    // Loop through each character in the string literal
    size_t index = 0;
    while(cstr[index] != '\0')
    {
        // Parse and store the character.
        index += internal[_elements++].parseFromString(cstr, index);
    }
    return *this;
}

onestring& onestring::assign(const std::string& str)
{
    assign(str.c_str());
    return *this;
}

onestring& onestring::assign(const onestring& ostr)
{
    clear();
    reserve(ostr._elements);
    for(size_t i = 0; i < ostr._elements; ++i)
    {
        this->internal[i] = ostr.internal[i];
    }
    _elements = ostr._elements;
    return *this;
}

void onestring::clear()
{
    if (_elements > 0)
    {
        delete[] this->internal;
        internal = nullptr;
        _capacity = 0;
        reserve(BASE_SIZE);
        _elements = 0;
    }
}

onestring& onestring::erase(size_t pos, size_t len)
{
    if (pos > _elements)
    {
        throw std::out_of_range("Onestring::erase(): Index out of bounds.");
    }

    // Calculate the number of elements we need to REMOVE
    len = (len > _elements - pos) ? (_elements - pos) : len;

    // If there are any elements remaining after the erasure...
    if(_elements - pos - len > 0)
    {
        // Move the remaining elements back
        shift(pos + len, -(len));
    }

    // Update the number of elements
    _elements = _elements - len;

    return *this;
}

std::istream& onestring::getline(std::istream& is, onestring& ostr, char delim)
{
    ostr.clear();

    char ch;
    while (is.get(ch) && ch != delim)
    {
        ostr.append(ch);
    }
    return is;
}

onestring& onestring::insert(size_t pos, char ch)
{
    // Ensure the insertion position is in range.
    if (pos >= this->_elements)
    {
        throw std::out_of_range("Onestring::insert(): specified pos out of range");
    }

    // Reserve needed space
    expand(1);
    // Move the right partition to make room for the new element
    shift(pos, 1);

    // Insert the new element
    this->internal[pos] = ch;
    // Increase the element count
    ++_elements;

    return *this;
}

onestring& onestring::insert(size_t pos, onechar& ochr)
{
    // Ensure the insertion position is in range.
    if (pos >= this->_elements)
    {
        throw std::out_of_range("Onestring::insert(): specified pos out of range");
    }

    // Reserve needed space
    expand(1);
    // Move the right partition to make room for the new element
    shift(pos, 1);

    // Insert the new element
    this->internal[pos] = ochr;
    // Increase the element count
    ++_elements;

    return *this;
}

onestring& onestring::insert(size_t pos, char* cstr)
{
    // Ensure the insertion position is in range.
    if (pos >= this->_elements)
    {
        throw std::out_of_range("Onestring::insert(): specified pos out of range");
    }

    size_t elements_to_insert = characterCount(cstr);
    // Reserve needed space
    expand(elements_to_insert);
    // Move the right partition to make room for the new element
    shift(pos, elements_to_insert);

    // Insert the new elements
    size_t index = 0;
    while(cstr[index] != '\0')
    {
        // Parse and store the character.
        index += internal[pos++].parseFromString(cstr, index);
    }
    // Increase the element count
    _elements += elements_to_insert;

    return *this;
}

onestring& onestring::insert(size_t pos, std::string& str)
{
    return insert(pos, str.c_str());
}

onestring& onestring::insert(size_t pos, const onestring& ostr)
{
    // Ensure the insertion position is in range.
    if (pos >= this->_elements)
    {
        throw std::out_of_range("Onestring::insert(): specified pos out of range");
    }

    size_t elements_to_insert = ostr._elements;
    // Reserve needed space
    expand(elements_to_insert);
    // Move the right partition to make room for the new element
    shift(pos, elements_to_insert);

    // Insert the new elements
    for(size_t i = 0; i < ostr._elements; ++i)
    {
        this->internal[pos + i] = ostr.internal[i];
    }
    // Increase the element count
    _elements += elements_to_insert;

    return *this;
}

void onestring::pop_back()
{
    if(_elements > 0)
    {
        /* We don't actually need to delete anything. The space will be
        * reused or deallocated as needed by other functions. */
        --_elements;
    }
}

void onestring::replace_setup(size_t pos, size_t len, size_t sublen)
{
    // WARNING: pos and len are not validated. That MUST be done externally!

    // Calculate whether we're replacing 1:1, expanding, or erasing
    int difference = sublen - len;
    size_t pos_after = pos + len;
    size_t elements_after = _elements - pos_after;

    // If our replacement expands the string...
    if (difference > 0)
    {
        // Reserve the needed space
        expand(difference);

        // Move the right partition to make room, if necessary
        if (elements_after > 0)
        {
            shift(pos, difference);
        }

        // Update the element count
        _elements += difference;
    }
    // Otherwise, if our replacement shrinks the string...
    else if (difference < 0)
    {
        // Move the right partition back, erasing excess characters.
        if (elements_after > 0)
        {
            shift(pos_after, difference);
        }

        // Update the element count
        _elements += difference;
    }

    // If the string size won't change, we need no extra setup.
}

onestring& onestring::replace(size_t pos, size_t len, const char ch)
{
    // Ensure the replacement position is in range.
    if (pos >= this->_elements)
    {
        throw std::out_of_range("Onestring::replace(): specified pos out of range");
    }

    // Adjust len so it doesn't run past the end of the string
    len = (len > _elements - pos) ? (_elements - pos) : len;

    // Rearrange the string memory to accommodate the new data
    replace_setup(pos, len, 1);

    // Write the data, using pos as our destination index.
    this->internal[pos] = ch;

    return *this;
}

onestring& onestring::replace(size_t pos, size_t len, const onechar& ochr)
{
    // Ensure the replacement position is in range.
    if (pos >= this->_elements)
    {
        throw std::out_of_range("Onestring::replace(): specified pos out of range");
    }

    // Adjust len so it doesn't run past the end of the string
    len = (len > _elements - pos) ? (_elements - pos) : len;

    // Rearrange the string memory to accommodate the new data
    replace_setup(pos, len, 1);

    // Write the data, using pos as our destination index.
    this->internal[pos] = ochr;

    return *this;
}

onestring& onestring::replace(size_t pos, size_t len, const char* cstr)
{
    // Ensure the replacement position is in range.
    if (pos >= this->_elements)
    {
        throw std::out_of_range("Onestring::replace(): specified pos out of range");
    }

    // Adjust len so it doesn't run past the end of the string
    len = (len > _elements - pos) ? (_elements - pos) : len;

    // Rearrange the string memory to accommodate the new data
    replace_setup(pos, len, characterCount(cstr));

    // Write the data, using pos as our destination index.
    size_t index = 0;
    while(cstr[index] != '\0')
    {
        // Parse and store the character.
        index += internal[pos++].parseFromString(cstr, index);
    }

    return *this;
}

onestring& onestring::replace(size_t pos, size_t len, const std::string& str)
{
    return replace(pos, len, str.c_str());
}

onestring& onestring::replace(size_t pos, size_t len, const onestring& ostr)
{
    // Ensure the replacement position is in range.
    if (pos >= this->_elements)
    {
        throw std::out_of_range("Onestring::replace(): specified pos out of range");
    }

    // Adjust len so it doesn't run past the end of the string
    len = (len > _elements - pos) ? (_elements - pos) : len;

    // Rearrange the string memory to accommodate the new data
    replace_setup(pos, len, ostr._elements);

    for(size_t i = 0; i < ostr._elements; ++i)
    {
        this->internal[pos + i] = ostr.internal[i];
    }

    return *this;
}

onestring& onestring::replace(size_t pos, size_t len, const char* cstr, size_t subpos, size_t sublen)
{
    // Ensure the replacement position is in range.
    if (pos >= this->_elements)
    {
        throw std::out_of_range("Onestring::replace(): specified pos out of range");
    }

    size_t cstr_len = strlen(cstr);
    if (subpos >= cstr_len)
    {
        throw std::out_of_range("Onestring::replace(): specified subpos out of range");
    }

    // Adjust len so it doesn't run past the end of the string
    len = (len > _elements - pos) ? (_elements - pos) : len;
    // Adjust sublen so it doesn't run past the end of the c-string.
    sublen = (sublen > cstr_len - subpos) ? (cstr_len - subpos) : sublen;

    // Rearrange the string memory to accommodate the new data
    replace_setup(pos, len, sublen);

    // Repurpose sublen so it marks the end of the area we are to copy from
    sublen += subpos;

    // Replace the characters in the specified range, using pos and subpos as our indices.
    while(subpos < sublen)
    {
        // Parse and store the character.
        subpos += internal[pos++].parseFromString(cstr, subpos);
    }

    return *this;
}

onestring& onestring::replace(size_t pos, size_t len, const std::string& str, size_t subpos, size_t sublen)
{
    return replace(pos, len, str.c_str(), subpos, sublen);
}

onestring& onestring::replace(size_t pos, size_t len, const onestring& ostr, size_t subpos, size_t sublen)
{
    // Ensure the replacement position is in range.
    if (pos >= this->_elements)
    {
        throw std::out_of_range("Onestring::replace(): specified pos out of range");
    }

    if (subpos >= ostr._elements)
    {
        throw std::out_of_range("Onestring::replace(): specified subpos out of range");
    }

    // Adjust len so it doesn't run past the end of the string
    len = (len > _elements - pos) ? (_elements - pos) : len;
    // Adjust sublen so it doesn't run past the end of the c-string.
    sublen = (sublen > ostr._elements - subpos) ? (ostr._elements - subpos) : sublen;

    // Rearrange the string memory to accommodate the new data
    replace_setup(pos, len, sublen);

    for(size_t i = 0; i < sublen; ++i)
    {
        this->internal[pos + i] = ostr.internal[subpos + i];
    }

    return *this;
}

onestring& onestring::reverse()
{
    // Create temporary storage unit
    onechar temp;

    // Perform the swap
    size_t lhe = 0;
    size_t rhe = _elements - 1;
    while(lhe < rhe)
    {
        temp = this->internal[rhe];
        this->internal[rhe] = this->internal[lhe];
        this->internal[lhe] = temp;
        ++lhe;
        --rhe;
    }

    return *this;
}

void onestring::swap(std::string& str)
{
    std::string temp = str;
    str = this->c_str();
    this->assign(temp);
}

void onestring::swap(onestring& ostr)
{
    onestring temp = ostr;
    ostr.assign(*this);
    this->assign(temp);
}

/** Combine a onestring and a char.
  * \param lhs: the operand on the left of the operator
  * \param rhs: the operand on the right of the operator
  * \return a new onestring object
  */

onestring operator+(const onestring& lhs, const char& rhs)
{
    onestring new_ostr(lhs);
    new_ostr.append(rhs);
    return new_ostr;
}

//Combines a char and a onestring
onestring operator+(const char& lhs, const onestring& rhs)
{
    onestring new_ostr(lhs);
    new_ostr.append(rhs);
    return new_ostr;
}

//Combines a onestring and a std::string
onestring operator+(const onestring& lhs, const std::string& rhs)
{
    onestring new_ostr(lhs);
    new_ostr.append(rhs);
    return new_ostr;
}

//Combines a std::string and a onestring
onestring operator+(const std::string& lhs, const onestring& rhs)
{
    onestring new_ostr(lhs);
    new_ostr.append(rhs);
    return new_ostr;
}

//Combines a onestring and a cstr
onestring operator+(const onestring& lhs, const char* rhs)
{
    onestring new_ostr(lhs);
    new_ostr.append(rhs);
    return new_ostr;
}

//Combines a cstr and a onestring
onestring operator+(const char* lhs, const onestring& rhs)
{
    onestring new_ostr(lhs);
    new_ostr.append(rhs);
    return new_ostr;
}

//Combines a onestring and a onestring
onestring operator+(const onestring& lhs, const onestring& rhs)
{
    onestring new_ostr(lhs);
    new_ostr.append(rhs);
    return new_ostr;
}
