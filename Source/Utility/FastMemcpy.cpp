/*
Copyright (C) 2012 Corn

aligned case & byte copy (except ASM) by
Copyright (C) 2009 Raphael
E-mail:   raphael@fx-world.org
homepage: http://wordpress.fx-world.org
*/

#include "stdafx.h"
#include "FastMemcpy.h"

//*****************************************************************************
//Copy native N64 memory with CPU only //Corn
//Little Endian
//*****************************************************************************
void memcpy_swizzle( void* dst, const void* src, size_t size )
{
	u8* src8 = (u8*)src;
	u8* dst8 = (u8*)dst;
	u32* src32;
	u32* dst32;

	// < 4 isn't worth trying any optimisations...
	if(size>=4)
	{
		// Align dst on 4 bytes or just resume if already done
		while (((((uintptr_t)dst8) & 0x3)!=0) )
		{
			*(u8*)((uintptr_t)dst8++ ^ U8_TWIDDLE) = *(u8*)((uintptr_t)src8++ ^ U8_TWIDDLE);
			size--;
		}

		// We are dst aligned now but need at least 4 bytes to copy
		if(size>=4)
		{
			src32 = (u32*)src8;
			dst32 = (u32*)dst8;
			u32 srcTmp;
			u32 dstTmp;

			switch( (uintptr_t)src8&0x3 )
			{
				case 0:	//Both src and dst are aligned to 4 bytes at this point
					{
						while (size&0xC)
						{
							*dst32++ = *src32++;
							size -= 4;
						}

						while (size>=16)
						{
#ifdef DAEDALUS_PSP
							asm(".set	push\n"				// save assembler option
								".set	noreorder\n"		// suppress reordering
								"lw	 $8,  0(%1)\n"			//
								"lw	 $9,  4(%1)\n"			//
								"lw	 $2,  8(%1)\n"			//
								"lw	 $3, 12(%1)\n"			//
								"sw	 $8,  0(%0)\n"			//
								"sw	 $9,  4(%0)\n"			//
								"sw	 $2,  8(%0)\n"			//
								"sw	 $3, 12(%0)\n"			//
								".set	pop\n"				// restore assembler option
								:"+r"(dst32),"+r"(src32)
								:
								:"$8","$9","$2","$3","memory"
								);
							dst32 += 4;
							src32 += 4;
							size -= 16;
#else
							*dst32++ = *src32++;
							*dst32++ = *src32++;
							*dst32++ = *src32++;
							*dst32++ = *src32++;
							size -= 16;
#endif
						}

						src8 = (u8*)src32;
					}
					break;

				case 1:	//Handle offset by 1
					{
						src32 = (u32*)((u32)src8 & ~0x3);
						srcTmp = *src32++;
						while(size>=4)
						{
							dstTmp = srcTmp << 8;
							srcTmp = *src32++;
							dstTmp |= srcTmp >> 24;
							*dst32++ = dstTmp;
							size -= 4;
						}
						src8 = (u8*)src32 - 3;
					}
					break;

				case 2:	//Handle offset by 2
					{
						src32 = (u32*)((u32)src8 & ~0x3);
						srcTmp = *src32++;
						while(size>=4)
						{
							dstTmp = srcTmp << 16;
							srcTmp = *src32++;
							dstTmp |= srcTmp >> 16;
							*dst32++ = dstTmp;
							size -= 4;
						}
						src8 = (u8*)src32 - 2;
					}
					break;

				case 3:	//Handle offset by 3
					{
						src32 = (u32*)((u32)src8 & ~0x3);
						srcTmp = *src32++;
						while(size>=4)
						{
							dstTmp = srcTmp << 24;
							srcTmp = *src32++;
							dstTmp |= srcTmp >> 8;
							*dst32++ = dstTmp;
							size -= 4;
						}
						src8 = (u8*)src32 - 1;
					}
					break;
			}
			dst8 = (u8*)dst32;
		}
	}

	// Copy the remaing byte by byte...
	while(size--)
	{
		*(u8*)((uintptr_t)dst8++ ^ U8_TWIDDLE) = *(u8*)((uintptr_t)src8++ ^ U8_TWIDDLE);
	}
}
