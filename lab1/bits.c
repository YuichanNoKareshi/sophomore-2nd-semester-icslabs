/* 
 * CS:APP Data Lab 
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#include "btest.h"
#include <limits.h>

/*
 * Instructions to Students:
 *
 * STEP 1: Fill in the following struct with your identifying info.
 */
team_struct team =
{
   /* Team name: Replace with either:
      Your login ID if working as a one person team
      or, ID1+ID2 where ID1 is the login ID of the first team member
      and ID2 is the login ID of the second team member */
    "518030910237", 
   /* Student name 1: Replace with the full name of first team member */
   "Zhou Yitian",
   /* Login ID 1: Replace with the login ID of first team member */
   "518030910237",

   /* The following should only be changed if there are two team members */
   /* Student name 2: Full name of the second team member */
   "",
   /* Login ID 2: Login ID of the second team member */
   ""
};

#if 0
/*
 * STEP 2: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.
#endif

/*
 * STEP 3: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the btest test harness to check that your solutions produce 
 *      the correct answers. Watch out for corner cases around Tmin and Tmax.
 */
/* Copyright (C) 1991-2016 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */
/* This header is separate from features.h so that the compiler can
   include it implicitly at the start of every compilation.  It must
   not itself include <features.h> or any other header that includes
   <features.h> because the implicit include comes before any feature
   test macros that may be defined in a source file before it first
   explicitly includes a system header.  GCC knows the name of this
   header in order to preinclude it.  */
/* glibc's intent is to support the IEC 559 math functionality, real
   and complex.  If the GCC (4.9 and later) predefined macros
   specifying compiler intent are available, use them to determine
   whether the overall intent is to support these features; otherwise,
   presume an older compiler has intent to support these features and
   define these macros by default.  */
/* wchar_t uses Unicode 9.0.0.  Version 9.0 of the Unicode Standard is
   synchronized with ISO/IEC 10646:2014, fourth edition, plus
   Amd. 1  and Amd. 2 and 273 characters from forthcoming  10646, fifth edition.
   (Amd. 2 was published 2016-05-01,
   see https://www.iso.org/obp/ui/#iso:std:iso-iec:10646:ed-4:v1:amd:2:v1:en) */
/* We do not support C11 <threads.h>.  */
/* 
 * bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int bang(int x) {
  x = x | (x >> 16);
  x = x | (x >> 8);
  x = x | (x >> 4);
  x = x | (x >> 2);
  x = x | (x >> 1);
  x = x & 0x1; 
  x = x ^ 0x1;
  return x;
}
/*
 * bitCount - returns count of number of 1's in word
 *   Examples: bitCount(5) = 2, bitCount(7) = 3
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 40
 *   Rating: 4
 */
int bitCount(int x) {
  int a;
  int b;
  int c;
  int result;
  
  a = 0x55;
  a = a | ( a << 8 );
  a = a | ( a << 16 );
  a = ( x & a ) + ( ( x >> 1 ) & a );

  b = 0x33;
  b = b | ( b << 8 );
  b = b | ( b << 16 );
  b = ( a & b ) + ( ( a >> 2 ) & b );

  c = 0x0F;
  c = c | ( c << 8 );
  c = c | ( c << 16 );
  c = ( b & c ) + ( ( b >> 4 ) & c );
  
  result = ( c & 0xFF ) + ( ( c >> 8 ) & 0xFF ) + ( ( c >> 16 ) & 0xFF ) + ( ( c >> 24 ) & 0xFF );
  return result;
}
/* 
 * copyLSB - set all bits of result to least significant bit of x
 *   Example: copyLSB(5) = 0xFFFFFFFF, copyLSB(6) = 0x00000000
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int copyLSB(int x) {
  int y;
  y = 0x1 & x ;
  y = y << 31 ;
  y = y >> 31 ;
  return y;
}
/* 
 * divpwr2 - Compute x/(2^n), for 0 <= n <= 30
 *  Round toward zero
 *   Examples: divpwr2(15,1) = 7, divpwr2(-33,4) = -2
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int divpwr2(int x, int n) {
    int temp;
    int head;
    int a1;
    int a2;
    
    temp = x;
    temp = temp >> 31;
    head = temp & 0x1;//if x < 0 ,head = 1
    
    a1 = 0x80;
    a1 = ~ ( a1 << 24 );
    a2 = a1 >> ( 32 + (~ n));//>> 31-n bits
    a2 = a2 & x;

    x = x >> n ;
    x = x + ( (! !a2) & head ); 
    return x;
}
/* 
 * evenBits - return word with all even-numbered bits set to 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 2
 */
int evenBits(void) {
  int a1;
  int a2;
  int a3;
  int a4;
  
  a1 = 0x55;
  a2 = a1 << 8 ;
  a3 = a2 << 8 ;
  a4 = a3 << 8 ;
  a1 = a1 | a2 | a3 | a4;
  return a1;
}
/* 
 * fitsBits - return 1 if x can be represented as an 
 *  n-bit, two's complement integer.
 *   1 <= n <= 32
 *   Examples: fitsBits(5,3) = 0, fitsBits(-4,3) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int fitsBits(int x, int n) {
  int head;
  int positive;
  int negative;
  int result1;
  int result2;
  int result;

  head = (x >> 31) & 0x01;

  positive = x >> (n + (~0x0));//>>n-1 , positive =0 if fitsbits,else > 0
  negative = ( x >> (n + (~0x0)) ) + 1;//if fitsbits, nagetive = 0, else <0

  result1 = (!head) & (!positive);
  result2 = head & (!negative);
  result = result1 | result2;
  return result;
}
/* 
 * getByte - Extract byte n from word x
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: getByte(0x12345678,1) = 0x56
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
int getByte(int x, int n) {
  int a;
  
  n = n << 3;//n*8
  a = 0xFF;
  x = x >> n;
  a = a & x;
  return a;
}
/* 
 * isGreater - if x > y  then return 1, else return 0 
 *   Example: isGreater(4,5) = 0, isGreater(5,4) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isGreater(int x, int y) {
  int heady;
  int headx;
  int y1;
  int result1;
  int result2;
  int positive;
  int result_x1_y2;
  int result_x2_y1;
  int result;

  heady = 0x1 & (y >> 31) ;//if y>0, head == 0
  headx = 0x1 & (x >> 31) ;//if x>0, head == 0
  y1 = ( ~ y ) + 1;//if y > 0, -y

  result1 = x + y1;
  result2 = ( result1 >> 31 ) & 0x01;
  positive = (!result2) & (!!result1);//=1 if result1>0
  result_x1_y2 = (!headx) & heady;//=1 iff x>0 && y<0
  result_x2_y1 = headx & (!heady);//=1 iff x<0 && y>0
  result = ( positive | result_x1_y2 ) & (!result_x2_y1);
  return result;
}
/* 
 * isNonNegative - return 1 if x >= 0, return 0 otherwise 
 *   Example: isNonNegative(-1) = 0.  isNonNegative(0) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 3
 */
int isNonNegative(int x) {
  int a1;
  int a2;

  x = x >> 1;
  a1 = 0x01;
  a2 = 0x40;
  a2 = a2 << 24;
  x = x & a2;
  x = x >> 30; 
  x = x ^ a1; 
  return x;
}
/* 
 * isNotEqual - return 0 if x == y, and 1 otherwise 
 *   Examples: isNotEqual(5,5) = 0, isNotEqual(4,5) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
int isNotEqual(int x, int y) {
  x = x ^ y;
  x = ! ! x;
  return x;
}
/*
 * isPower2 - returns 1 if x is a power of 2, and 0 otherwise
 *   Examples: isPower2(5) = 0, isPower2(8) = 1, isPower2(0) = 0
 *   Note that no negative number is a power of 2.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 60
 *   Rating: 4
 */
int isPower2(int x) {
  int temp;
  int a1;
  int a2;
  int a;
  int b;
  int c;
  int result;
  int p;

  temp = x;
  temp = temp >> 1;
  a1 = 0x01;
  a2 = 0x40;
  a2 = a2 << 24;
  temp = temp & a2;
  temp = temp >> 30; 
  temp = temp ^ a1; //isNonNegative(x),temp == 1 if x >= 0


  a = 0x55;
  a = a | ( a << 8 );
  a = a | ( a << 16 );
  a = ( x & a ) + ( ( x >> 1 ) & a );

  b = 0x33;
  b = b | ( b << 8 );
  b = b | ( b << 16 );
  b = ( a & b ) + ( ( a >> 2 ) & b );

  c = 0x0F;
  c = c | ( c << 8 );
  c = c | ( c << 16 );
  c = ( b & c ) + ( ( b >> 4 ) & c );
  
  result = ( c & 0xFF ) + ( ( c >> 8 ) & 0xFF ) + ( ( c >> 16 ) & 0xFF ) + ( ( c >> 24 ) & 0xFF );
  //bitCount(x), result = number of 1 in x
  
  p = result & ( !( result >> 1 ) ) & temp;
  return p;
}
/* 
 * leastBitPos - return a mask that marks the position of the
 *               least significant 1 bit. If x == 0, return 0
 *   Example: leastBitPos(96) = 0x20 ,96=0x01100000
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 4 
 */
int leastBitPos(int x) {
  int a;

  a = ~ x;
  a = a + 1;
  a = a & x;
  return a;
}
/* 
 * logicalShift - shift x to the right by n, using a logical shift
 *   Can assume that 1 <= n <= 31
 *   Examples: logicalShift(0x87654321,4) = 0x08765432
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3 
 */
int logicalShift(int x, int n) {
  int temp1;
  int temp2;
  int temp3;
  int temp4;
  int temp5;
  
  temp1 = 0x7F;
  temp1 = temp1 << 24;
  x = x >> n;
  temp1 =  temp1 >> n;
  temp1 = temp1 << 1;//youyi n-1 wei
  temp2 = ( 0xFF << 17 ) >> n;
  temp3 = ( 0xFF << 9 ) >> n;
  temp4= ( 0xFF << 1 ) >> n;
  temp5 = 0x1;
  temp1 = temp1 | temp2 | temp3 | temp4 | temp5;
  x = x & temp1;
  return x;
}
/*
 * satAdd - adds two numbers but when positive overflow occurs, returns
 *          maximum possible value, and when negative overflow occurs,
 *          it returns minimum positive value.
 *   Examples: satAdd(0x40000000,0x40000000) = 0x7fffffff
 *             satAdd(0x80000000,0xffffffff) = 0x80000000s
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 30
 *   Rating: 4
 */
int satAdd(int x, int y) {
  int sum;
  int head;
  int headx;
  int heady;
  int isTmax;
  int isTmin;
  int Tmax;
  int Tmin;
  int result;
  
  sum = x + y;
  head = sum >> 31;
  headx = x >> 31;
  heady = y >> 31;

  isTmax = head & (~headx) & (~heady);//== 0xFFFFFFFF iff Tmax
  isTmin = (~head) & headx & heady;//== 0xFFFFFFFF iff Tmin

  Tmax = ~ (0x80 << 24);
  Tmin = 0x80 << 24;

result = ((~isTmax) & (~isTmin) & sum) + (Tmax & isTmax) + (Tmin & isTmin);
  return result;////////////////////////////////////////////////////////////////////////
}
/* 
 * tc2sm - Convert from two's complement to sign-magnitude 
 *   where the MSB is the sign bit
 *   You can assume that x > TMin
 *   Example: tc2sm(-5) = 0x80000005.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 4
 */
int tc2sm(int x) {
  int head;
  int sm;
  int result;

  head = x >> 31;
  sm = ( 0x80 << 24 ) | ((~x)+1);

  result = ((~head) & x) | (head & sm);
  return result;
}
