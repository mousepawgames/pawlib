/** Tests for FlexArray[PawLIB]
  * Version: 1.0
  *
  * Author(s): Jonathan Theodore
  */

/* LICENSE
 * Copyright (c) 2016 MousePaw Games.
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
 * See http://www.mousepawgames.com/participate/opensource for information
 * on how to contribute to our projects.
 */

#ifndef FLEX_ARRAY_TEST_HPP
#define FLEX_ARRAY_TEST_HPP

#include <vector>

#include <flex_array.hpp>
#include <goldilocks.hpp>
#include <stdutils.hpp>

namespace pawlib
{
    // P-tB1001*
    class TestVector_Push : public Test
    {
        private:
            unsigned int iters;

        public:
            explicit TestVector_Push(unsigned int iterations)
                :iters(iterations)
                {}

            testdoc_t get_title()
            {
                return "FlexArray: Push " + stdutils::itos(iters, 10) + " Integers (std::vector)";
            }

            testdoc_t get_docs()
            {
                return "Insert " + stdutils::itos(iters, 10) + " integers at the back of a vector (push).";
            }

            bool run()
            {
                //Create instance of vector.
                std::vector<unsigned int> vec;

                // Insert each required element via a push.
                for(unsigned int i=0; i<iters; ++i)
                {
                    vec.push_back(i);
                }
                return true;
            }

            ~TestVector_Push(){}

    };

    // P-tB1001, P-tS1001
    class TestFArray_Push : public Test
    {
        private:
                unsigned int iters;

        public:
            explicit TestFArray_Push(unsigned int iterations)
                :iters(iterations)
                {}

            testdoc_t get_title()
            {
                return "FlexArray: Push " + stdutils::itos(iters, 10) + " Integers (FlexArray)";
            }
            testdoc_t get_docs()
            {
                return "Insert " + stdutils::itos(iters, 10) + " integers at the back of a FlexArray (push).";
            }

            bool run()
            {
                // Create instance of FlexArray.
                pawlib::FlexArray< unsigned int> flex;

                // Insert each required element via a push.
                for(unsigned int i=0; i<iters; ++i)
                {
                    // Attempt a push. If it fails...
                    if(!flex.push(i))
                    {
                        // Report failure.
                        return false;
                    }
                }
                // Report success.
                return true;
            }

            ~TestFArray_Push(){}

    };

    // P-tB1002*
    class TestVector_Shift : public Test
    {
        public:
           explicit TestVector_Shift(unsigned int iterations): iters(iterations){}

            testdoc_t get_title()
            {
                return "FlexArray: Shift " + stdutils::itos(iters, 10) + " Integers to Front (std::vector)";
            }

            testdoc_t get_docs()
            {
                return "Insert " + stdutils::itos(iters, 10) + " integers at the front of a vector (shift).";
            }

            bool run()
            {
                // Create instance of vector.
                std::vector<unsigned int> vec;

                // Insert each required element via a shift.
                for(unsigned int i=0; i<iters; ++i)
                {
                    vec.insert(vec.begin(), i);
                }
                return true;
            }

            ~TestVector_Shift(){}

            private:
                unsigned int iters;
     };

    // P-tB1002, P-tS1002
    class TestFArray_Shift: public Test
    {
        private:
            unsigned int iters;

        public:
            explicit TestFArray_Shift(unsigned int iterations)
                :iters(iterations)
                {}

            testdoc_t get_title()
            {
                return "FlexArray: Shift " + stdutils::itos(iters, 10) + " Integers to Front (FlexArray)";
            }

            testdoc_t get_docs()
            {
                return "Insert " + stdutils::itos(iters, 10) + " integers at the front of a FlexArray (shift).";
            }

            bool run()
            {
                // Create instance of FlexArray.
                pawlib::FlexArray<unsigned int> flex;

                // Insert each required element via a shift.
                for(unsigned int i=0; i<iters; ++i)
                {
                    // Attempt a shift. If it fails...
                    if(!flex.shift(i))
                    {
                        // Report failure.
                        return false;
                    }
                }
                return true;
            }

            ~TestFArray_Shift(){}

    };

    //P-tB1003*
    class TestVector_Insert : public Test
    {
        private:
            unsigned int iters;

        public:
            explicit TestVector_Insert(unsigned int iterations)
                :iters(iterations)
                {}

            testdoc_t get_title()
            {
                return "FlexArray: Insert " + stdutils::itos(iters, 10) + " Integers At Middle (std::vector)";
            }

            testdoc_t get_docs()
            {
                return "Insert " + stdutils::itos(iters, 10) + " integers at the middle of a vector. Middle \
is calculated as size()/2.";
            }

            bool run()
            {
                // Create an instance of vector.
                std::vector< unsigned int> vec;

                // We initially push two values, to make the
                // math calculating the insert index a bit safer.
                vec.push_back(0);
                vec.push_back(1);

                // Insert each required element.
                for(unsigned int i=1; i<1000; ++i)
                {
                    unsigned int j = vec.size()/2;
                    vec.insert(vec.begin()+j, i);
                }
                return true;
            }

            ~TestVector_Insert(){}

    };

    // P-tB1003, P-tS1003
    class TestFArray_Insert : public Test
    {
        private:
            unsigned int iters;


        public:
            explicit TestFArray_Insert(unsigned int iterations)
                :iters(iterations)
                {}

            testdoc_t get_title()
            {
                return "FlexArray: Insert " + stdutils::itos(iters, 10) + " Integers At Middle (flexarray)";
            }

            testdoc_t get_docs()
            {
                return "Insert " + stdutils::itos(iters, 10) + " integers at the middle of a flexarray. \
Middle is calculated as size()/2.";
            }

            bool run()
            {
                // Create instance of FlexArray.
                pawlib::FlexArray<unsigned int> flex;

                /* We initially push two values, to make the
                 * math calculating the insert index a bit safer.*/
                flex.push(0);
                flex.push(1);

                // Insert each required element.
                for(unsigned int i=1; i<iters; ++i)
                {
                    int j = flex.getSize()/2;
                    // Attempt an insert shift. If it fails...
                    if(!flex.insert(j, i))
                    {
                        // Report failure.
                        return false;
                    }
                }

                // Report success.
                return true;
            }

            ~TestFArray_Insert(){}

    };

    // P-tB1004*
    class TestVector_Erase : public Test
    {
        private:
            std::vector<unsigned int> vec;

            unsigned int iters;

        public:
          explicit TestVector_Erase(unsigned int iterations)
            :iters(iterations)
            {}

            testdoc_t get_title()
            {
                return "FlexArray: Erase " + stdutils::itos(iters, 10) + " Integers (std::vector)";
            }

            testdoc_t get_docs()
            {
                return "Erase " + stdutils::itos(iters, 10) + " integers at the front of a Vector (pop).";
            }

            bool pre()
            {
                return janitor();
            }

            bool janitor()
            {
                // Fill up the vector.
                for(unsigned int i=0; i<iters; ++i)
                {
                    vec.push_back(i);
                }
                return true;
            }

            bool run()
            {
                // Remove and return each element.
                for(unsigned int i=0; i<iters; ++i)
                {
                    // Store the back element.
                    unsigned int temp = vec.back();

                    if(temp != vec.back())
                    {
                        // Can't happen. If it does, everything is weird.
                        return false;
                    }

                    // Remove the element.
                    vec.pop_back();
                }
                return true;
            }

            ~TestVector_Erase(){}
    };

    // P-tB1004, P-tS1004
    class TestFArray_Yank : public Test
    {
        private:
            pawlib::FlexArray<unsigned int> flex;

            unsigned int iters;

        public:
            explicit TestFArray_Yank(unsigned int iterations)
                :iters(iterations)
                {}

            testdoc_t get_title()
            {
                return "FlexArray: Yank " + stdutils::itos(iters, 10) + " Integers (flexarray)";
            }

            testdoc_t get_docs()
            {
                return "Erase " + stdutils::itos(iters, 10) + " integers at any given index (yank).";
            }

            bool pre()
            {
                return janitor();
            }

            bool janitor()
            {
                // Refill the FlexArray.
                for(unsigned int i=0; i<iters; ++i)
                {
                    flex.push(i);
                }
                return true;
            }

            bool run()
            {
                // For each element...
                for(int i=(iters-1); i>=0; --i)
                {
                    // Store the element at i.
                    unsigned int temp = flex.at(i);

                    // If the yanked element isn't what it should be...
                    if(flex.yank(i) != temp)
                    {
                        // Report failure.
                        return false;
                    }
                }
                return true;
            }
        ~TestFArray_Yank(){}
    };

    class TestSuite_FlexArray : public TestSuite
    {
        public:
            explicit TestSuite_FlexArray(){}

            void load_tests();

            testdoc_t get_title()
            {
                return "PawLIB: FlexArray Tests";
            }

            ~TestSuite_FlexArray(){}
    };
};
#endif // FLEX_ARRAY_TEST_HPP
