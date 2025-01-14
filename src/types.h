/*
	Copyright (C) 2005 Guillaume Duhamel
	Copyright (C) 2008-2015 DeSmuME team

	This file is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option) any later version.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with the this software.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TYPES_HPP
#define TYPES_HPP

#include <retro_miscellaneous.h>
#include <retro_inline.h>
#include <math/fxp.h>

//analyze microsoft compilers
#ifdef _MSC_VER
	#define HOST_WINDOWS
#endif //_MSC_VER

// Determine CPU architecture for platforms that don't use the autoconf script
#if defined(HOST_WINDOWS) || defined(DESMUME_COCOA)
	#if defined(__x86_64__) || defined(__LP64) || defined(__IA64__) || defined(_M_X64) || defined(_WIN64)
		#define HOST_64
	#else
		#define HOST_32
	#endif
#endif

//enforce a constraint: gdb stub requires developer
#if defined(GDB_STUB) && !defined(DEVELOPER)
	#define DEVELOPER
#endif

#ifdef DEVELOPER
	#define IF_DEVELOPER(X) X
#else
	#define IF_DEVELOPER(X)
#endif

#ifdef __GNUC__
	#ifdef __SSE__
		#define ENABLE_SSE
	#endif

	#ifdef __SSE2__
		#define ENABLE_SSE2
	#endif

	#ifdef __SSE3__
		#define ENABLE_SSE3
	#endif

	#ifdef __SSSE3__
		#define ENABLE_SSSE3
	#endif

	#ifdef __SSE4_1__
		#define ENABLE_SSE4_1
	#endif

	#ifdef __SSE4_2__
		#define ENABLE_SSE4_2
	#endif

	#ifdef __AVX__
		#define ENABLE_AVX
	#endif

	#ifdef __AVX2__
		#define ENABLE_AVX2
	#endif

	#ifdef __ALTIVEC__
		#define ENABLE_ALTIVEC
	#endif
#endif

#ifdef _MSC_VER 
	#include <compat/msvc.h>

#else
	#define WINAPI
#endif

#if !defined(MAX_PATH)
	#if defined(HOST_WINDOWS)
		#define MAX_PATH 260
	#elif defined(__GNUC__)
		#include <limits.h>
		#if !defined(PATH_MAX)
			#define MAX_PATH 1024
		#else
			#define MAX_PATH PATH_MAX
		#endif
	#else
		#define MAX_PATH 1024
	#endif
#endif

//------------alignment macros-------------
//dont apply these to types without further testing. it only works portably here on declarations of variables
//cant we find a pattern other people use more successfully?
#if _MSC_VER >= 1900
	#define DS_ALIGN(X) alignas(X)
#elif defined(_MSC_VER) || defined(__INTEL_COMPILER)
	#define DS_ALIGN(X) __declspec(align(X))
#elif defined(__GNUC__)
	#define DS_ALIGN(X) __attribute__ ((aligned (X)))
#else
	#define DS_ALIGN(X)
#endif

#ifdef HOST_64
	#define CACHE_ALIGN_SIZE 64
#else
	#define CACHE_ALIGN_SIZE 32
#endif

//use this for example when you want a byte value to be better-aligned
#define CACHE_ALIGN DS_ALIGN(CACHE_ALIGN_SIZE)
#define FAST_ALIGN DS_ALIGN(4)
//---------------------------------------------

#ifdef __MINGW32__
	#define FASTCALL __attribute__((fastcall))
	#define ASMJIT_CALL_CONV kX86FuncConvGccFastCall
#elif defined (__i386__) && !defined(__clang__)
	#define FASTCALL __attribute__((regparm(3)))
	#define ASMJIT_CALL_CONV kX86FuncConvGccRegParm3
#elif defined(_MSC_VER) || defined(__INTEL_COMPILER)
	#define FASTCALL
	#define ASMJIT_CALL_CONV kX86FuncConvDefault
#else
	#define FASTCALL
	#define ASMJIT_CALL_CONV kX86FuncConvDefault
#endif

#ifdef _MSC_VER
	#define _CDECL_ __cdecl
#else
	#define _CDECL_
#endif

#ifndef FORCEINLINE
	#if defined(_MSC_VER) || defined(__INTEL_COMPILER)
		#define FORCEINLINE __forceinline
		#define MSC_FORCEINLINE __forceinline
	#else
		#define FORCEINLINE inline __attribute__((always_inline))
		#define MSC_FORCEINLINE
	#endif
#endif

#ifndef NOINLINE
	#ifdef __GNUC__
		#define NOINLINE __attribute__((noinline))
	#else
		#define NOINLINE
	#endif
#endif

#ifndef LOOPVECTORIZE_DISABLE
	#if defined(_MSC_VER)
		#if _MSC_VER >= 1700
			#define LOOPVECTORIZE_DISABLE loop(no_vector)
		#else
			#define LOOPVECTORIZE_DISABLE 
		#endif
	#elif defined(__clang__)
		#define LOOPVECTORIZE_DISABLE clang loop vectorize(disable)
	#else
		#define LOOPVECTORIZE_DISABLE
	#endif
#endif

#if defined(__LP64__)
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long DeSmumeU64;

typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long DeSmumeS64;
#else
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
#if defined(_MSC_VER) || defined(__INTEL_COMPILER)
typedef unsigned __int64 u64;
#else
typedef unsigned long long u64;
#endif

typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
#if defined(_MSC_VER) || defined(__INTEL_COMPILER)
typedef __int64 s64;
#else
typedef signed long long s64;
#endif
#endif

typedef u8  uint8;
typedef u16 uint16;

#ifndef OBJ_C
typedef u32 uint32;
#else
#define uint32 u32 //uint32 is defined in Leopard somewhere, avoid conflicts
#endif

#ifdef ENABLE_ALTIVEC
	#ifndef __APPLE_ALTIVEC__
		#include <altivec.h>
	#endif
typedef vector unsigned char v128u8;
typedef vector signed char v128s8;
typedef vector unsigned short v128u16;
typedef vector signed short v128s16;
typedef vector unsigned int v128u32;
typedef vector signed int v128s32;
#endif

#ifdef ENABLE_SSE2
#include <emmintrin.h>
typedef __m128i v128u8;
typedef __m128i v128s8;
typedef __m128i v128u16;
typedef __m128i v128s16;
typedef __m128i v128u32;
typedef __m128i v128s32;
#endif

#ifdef ENABLE_AVX2
#include <immintrin.h>
typedef __m256i v256u8;
typedef __m256i v256s8;
typedef __m256i v256u16;
typedef __m256i v256s16;
typedef __m256i v256u32;
typedef __m256i v256s32;
#endif

/*---------- GPU3D fixed-points types -----------*/

typedef s32 f32;
#define inttof32(n)          ((n) << 12)
#define f32toint(n)          ((n) >> 12)
#define floattof32(n)        ((int32)((n) * (1 << 12)))
#define f32tofloat(n)        (((float)(n)) / (float)(1<<12))

typedef s16 t16;
#define f32tot16(n)          ((t16)(n >> 8))
#define inttot16(n)          ((n) << 4)
#define t16toint(n)          ((n) >> 4)
#define floattot16(n)        ((t16)((n) * (1 << 4)))
#define t16ofloat(n)         (((float)(n)) / (float)(1<<4))

typedef s16 v16;
#define inttov16(n)          ((n) << 12)
#define f32tov16(n)          (n)
#define floattov16(n)        ((v16)((n) * (1 << 12)))
#define v16toint(n)          ((n) >> 12)
#define v16tofloat(n)        (((float)(n)) / (float)(1<<12))

typedef s16 v10;
#define inttov10(n)          ((n) << 9)
#define f32tov10(n)          ((v10)(n >> 3))
#define v10toint(n)          ((n) >> 9)
#define floattov10(n)        ((v10)((n) * (1 << 9)))
#define v10tofloat(n)        (((float)(n)) / (float)(1<<9))

/*----------------------*/

#ifndef OBJ_C
typedef int BOOL;
#else
//apple also defines BOOL
typedef int desmume_BOOL;
#define BOOL desmume_BOOL
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/* little endian (ds' endianess) to local endianess convert macros */
#ifdef MSB_FIRST	/* local arch is big endian */
# define LE_TO_LOCAL_16(x) ((((x)&0xff)<<8)|(((x)>>8)&0xff))
# define LE_TO_LOCAL_32(x) ((((x)&0xff)<<24)|(((x)&0xff00)<<8)|(((x)>>8)&0xff00)|(((x)>>24)&0xff))
# define LE_TO_LOCAL_64(x) ((((x)&0xff)<<56)|(((x)&0xff00)<<40)|(((x)&0xff0000)<<24)|(((x)&0xff000000)<<8)|(((x)>>8)&0xff000000)|(((x)>>24)&0xff0000)|(((x)>>40)&0xff00)|(((x)>>56)&0xff))
# define LOCAL_TO_LE_16(x) ((((x)&0xff)<<8)|(((x)>>8)&0xff))
# define LOCAL_TO_LE_32(x) ((((x)&0xff)<<24)|(((x)&0xff00)<<8)|(((x)>>8)&0xff00)|(((x)>>24)&0xff))
# define LOCAL_TO_LE_64(x) ((((x)&0xff)<<56)|(((x)&0xff00)<<40)|(((x)&0xff0000)<<24)|(((x)&0xff000000)<<8)|(((x)>>8)&0xff000000)|(((x)>>24)&0xff0000)|(((x)>>40)&0xff00)|(((x)>>56)&0xff))
#else		/* local arch is little endian */
# define LE_TO_LOCAL_16(x) (x)
# define LE_TO_LOCAL_32(x) (x)
# define LE_TO_LOCAL_64(x) (x)
# define LOCAL_TO_LE_16(x) (x)
# define LOCAL_TO_LE_32(x) (x)
# define LOCAL_TO_LE_64(x) (x)
#endif

// kilobytes and megabytes macro
#define MB(x) ((x)*1024*1024)
#define KB(x) ((x)*1024)

#define CPU_STR(c) ((c==ARM9)?"ARM9":"ARM7")
typedef enum
{
	ARM9 = 0,
	ARM7 = 1
} cpu_id_t;

inline DeSmumeU64 double_to_u64(double d)
{
	union
	{
		DeSmumeU64 a;
		double b;
	} fuxor;
	
	fuxor.b = d;
	return fuxor.a;
}

inline double u64_to_double(DeSmumeU64 u)
{
	union
	{
		DeSmumeU64 a;
		double b;
	} fuxor;
	
	fuxor.a = u;
	return fuxor.b;
}

inline u32 float_to_u32(float f)
{
	union
	{
		u32 a;
		float b;
	} fuxor;
	
	fuxor.b = f;
	return fuxor.a;
}

inline float u32_to_float(u32 u)
{
	union
	{
		u32 a;
		float b;
	} fuxor;
	
	fuxor.a = u;
	return fuxor.b;
}

//fairly standard for loop macros
#define MACRODO1(TRICK,TODO) { const size_t X = TRICK; TODO; }
#define MACRODO2(X,TODO)   { MACRODO1((X),TODO)   MACRODO1(((X)+1),TODO) }
#define MACRODO4(X,TODO)   { MACRODO2((X),TODO)   MACRODO2(((X)+2),TODO) }
#define MACRODO8(X,TODO)   { MACRODO4((X),TODO)   MACRODO4(((X)+4),TODO) }
#define MACRODO16(X,TODO)  { MACRODO8((X),TODO)   MACRODO8(((X)+8),TODO) }
#define MACRODO32(X,TODO)  { MACRODO16((X),TODO)  MACRODO16(((X)+16),TODO) }
#define MACRODO64(X,TODO)  { MACRODO32((X),TODO)  MACRODO32(((X)+32),TODO) }
#define MACRODO128(X,TODO) { MACRODO64((X),TODO)  MACRODO64(((X)+64),TODO) }
#define MACRODO256(X,TODO) { MACRODO128((X),TODO) MACRODO128(((X)+128),TODO) }

//this one lets you loop any number of times (as long as N<256)
#define MACRODO_N(N,TODO) {\
	if((N)&0x100) MACRODO256(0,TODO); \
	if((N)&0x080) MACRODO128((N)&(0x100),TODO); \
	if((N)&0x040) MACRODO64((N)&(0x100|0x080),TODO); \
	if((N)&0x020) MACRODO32((N)&(0x100|0x080|0x040),TODO); \
	if((N)&0x010) MACRODO16((N)&(0x100|0x080|0x040|0x020),TODO); \
	if((N)&0x008) MACRODO8((N)&(0x100|0x080|0x040|0x020|0x010),TODO); \
	if((N)&0x004) MACRODO4((N)&(0x100|0x080|0x040|0x020|0x010|0x008),TODO); \
	if((N)&0x002) MACRODO2((N)&(0x100|0x080|0x040|0x020|0x010|0x008|0x004),TODO); \
	if((N)&0x001) MACRODO1((N)&(0x100|0x080|0x040|0x020|0x010|0x008|0x004|0x002),TODO); \
}

//---------------------------
//Binary constant generator macro By Tom Torfs - donated to the public domain

//turn a numeric literal into a hex constant
//(avoids problems with leading zeroes)
//8-bit constants max value 0x11111111, always fits in unsigned long
#define HEX__(n) 0x##n##LU

//8-bit conversion function 
#define B8__(x) ((x&0x0000000FLU)?1:0) \
+((x&0x000000F0LU)?2:0) \
+((x&0x00000F00LU)?4:0) \
+((x&0x0000F000LU)?8:0) \
+((x&0x000F0000LU)?16:0) \
+((x&0x00F00000LU)?32:0) \
+((x&0x0F000000LU)?64:0) \
+((x&0xF0000000LU)?128:0)

//for upto 8-bit binary constants
#define B8(d) ((unsigned char)B8__(HEX__(d)))

// for upto 16-bit binary constants, MSB first
#define B16(dmsb,dlsb) (((unsigned short)B8(dmsb)<<8) \
+ B8(dlsb))

// for upto 32-bit binary constants, MSB first */
#define B32(dmsb,db2,db3,dlsb) (((unsigned long)B8(dmsb)<<24) \
+ ((unsigned long)B8(db2)<<16) \
+ ((unsigned long)B8(db3)<<8) \
+ B8(dlsb))

//Sample usage:
//B8(01010101) = 85
//B16(10101010,01010101) = 43605
//B32(10000000,11111111,10101010,01010101) = 2164238933
//---------------------------

#ifndef CTASSERT
#define	CTASSERT(x)		typedef char __assert ## y[(x) ? 1 : -1]
#endif

static const char hexValid[23] = {"0123456789ABCDEFabcdef"};


template<typename T> inline void reconstruct(T* t) { 
	t->~T();
	new(t) T();
}

/* fixed point speedup macros */


FORCEINLINE s32 sfx32_shiftdown(const DeSmumeS64 a)
{
	//TODO: replace me with direct calls to sfx32_shiftdown
	return fx32_shiftdown(a);
}

#endif
