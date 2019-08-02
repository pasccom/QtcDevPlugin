/* Copyright 2015-2016 Pascal COMBES <pascom@orange.fr>
 * 
 * This file is part of QtcDevPlugin.
 * 
 * QtcDevPlugin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * QtcDevPlugin is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with QtcDevPlugin. If not, see <http://www.gnu.org/licenses/>
 */

#ifndef QT_TEST_SUB_FUNCTION_H
#define QT_TEST_SUB_FUNCTION_H

#define SUB_TEST_FUNCTION_SUCESS_INDICATOR __mSuccess
#define SUB_TEST_FUNCTION_SKIP_INDICATOR __mSkip

#define HAS_SUB_TEST_FUNCTIONS                                                 \
    bool SUB_TEST_FUNCTION_SUCESS_INDICATOR;                                   \
    bool SUB_TEST_FUNCTION_SKIP_INDICATOR;                                     \

#define BEGIN_SUB_TEST_FUNCTION                                                \
    SUB_TEST_FUNCTION_SUCESS_INDICATOR = false;                                \
    SUB_TEST_FUNCTION_SKIP_INDICATOR = false;                                  \

#define END_SUB_TEST_FUNCTION                                                  \
    do {                                                                       \
        SUB_TEST_FUNCTION_SUCESS_INDICATOR = true;                             \
    } while(false);                                                            \

#define SUB_TEST_FUNCTION_SKIP(__msg__)                                        \
    do {                                                                       \
        SUB_TEST_FUNCTION_SKIP_INDICATOR = true;                               \
        QSKIP(__msg__);                                                        \
    } while(false);                                                            \

#define SUB_TEST_FUNCTION(__fun__)                                             \
    do {                                                                       \
        __fun__;                                                               \
        if (SUB_TEST_FUNCTION_SKIP_INDICATOR)                                  \
            QSKIP("Following sub-test function skipped: " #__fun__);           \
        if (!SUB_TEST_FUNCTION_SUCESS_INDICATOR)                               \
            QFAIL("Following sub-test function failed: " #__fun__);            \
        else                                                                   \
            SUB_TEST_FUNCTION_SUCESS_INDICATOR = false;                        \
    } while(false)                                                             \

#endif // QT_TEST_SUB_FUNCTION_H
