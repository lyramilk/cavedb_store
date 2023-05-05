#ifndef _lyramilk_cavedb_store_pack_h_
#define _lyramilk_cavedb_store_pack_h_

#include <string>
#include <string.h>

// namespace lyramilk::data::cavedb
namespace lyramilk{ namespace data { namespace cavedb {
	class pack
	{
		char* p;
		unsigned long long l;
	public:
		unsigned long needbytes()
		{
			return int64packsize(l) + l;
		}


		/*
			把数据写入到buff中，返回写入的字节长度，通过pbase返回写入的字符串头指针。
		*/
		unsigned long write(char* dest,unsigned long long destbuffsizee,char** pbase,bool revers)
		{
			unsigned long z = int64packsize(l);
			if(destbuffsize < l + z){
				return l + z;
			};

			unsigned long offset = 0;
			if(reverse){
				offset = int64pack_reverse(l,dest);
			}else{
				offset = int64pack(l,dest);
			}
			*pbase = dest + offset;
			memcpy(dest + offset,p,l);
			return l + z;
		}

		std::string str()
		{
			return std::string(p,l);
		}
	public:
		/*
			p是整个buff的最开始
		*/
		static pack read(char* p,unsigned long long l)
		{
			pack r;
			unsigned long offset = unpackint64(p,&r.l);
			r.p = p + offset;
			return r;
		}
		/*
			与read有区别，p是数据部分的开始，p不是整个buff的开始，而是中间。这个是为了申请内存后通过申请到的内存指针向前读取块的长度用的。
		*/
		static pack read_reverse(char* p,unsigned long long l)
		{
			pack r;
			unsigned long offset = unpackint64(p,&r.l);
			r.p = p;
			return r;
		}
		static unsigned long int64packsize(unsigned long long l)
		{
			if(l < 0x80){
				return 1;
			}
			if(l < 0x4000){
				return 2;
			}
			if(l < 0x200000){
				return 3;
			}
			if(l < 0x10000000){
				return 4;
			}
			if(l < 0x800000000ull){
				return 5;
			}
			if(l < 0x40000000000ull){
				return 6;
			}
			if(l < 0x2000000000000ull){
				return 7;
			}
			if(l < 0x100000000000000ull){
				return 8;
			}
			if(l < 0x8000000000000000ull){
				return 9;
			}
			
			// 上面都处理完已经63位了，此时如果扩展位为1，说明第64位也是1，不需要再用一个单独的字节装它了。
			return 9;
		}

		static unsigned long int64pack(unsigned long long l,char* pbuff)
		{
			unsigned long i = 0;
			for(;i<9;++i){
				char c = l&0x7f;
				l >>= 7;
				if(l > 0){
					c |= 0x80;
					pbuff[i] = c;
				}else{
					pbuff[i] = c;
					break;
				}
			}
			if(i == 9) return 9;
			return i + 1;
		}

		static unsigned long unpackint64(const char* pbuff,unsigned long long* l)
		{
			unsigned long long r = 0;
			unsigned long i;
			unsigned long q = 0;
			const unsigned char* p = (const unsigned char*)pbuff;
			for(i=0,q=0;i<9;++i,q+=7){
				unsigned long long b = p[i];
				if(0x80&b){
					r |= b << q;
				}else{
					r |= b << q;
					break;
				}
			}
			if(i == 9){
				r |= 0x8000000000000000ull;
			}
			*l = r;
			return i + 1;
		}

		static unsigned long int64pack_reverse(unsigned long long l,char* pbuff)
		{
			unsigned long ii = int64packsize(l);
			unsigned long i = 0;
			for(;i<9;++i){
				char c = l&0x7f;
				l >>= 7;
				if(l > 0){
					c |= 0x80;
					pbuff[ii - i - 1] = c;
				}else{
					pbuff[ii - i - 1] = c;
					break;
				}
			}
			if(i == 9) return 9;
			return i + 1;
		}

		static unsigned long unpackint64_reverse(const char* pbuff,unsigned long long* l)
		{
			unsigned long long r = 0;
			long i;
			unsigned long q = 0;
			const unsigned char* p = (const unsigned char*)pbuff;
			for(i=0,q=0;i<9;++i,q+=7){
				unsigned long long b = p[0 - i];
				if(0x80&b){
					r |= b << q;
				}else{
					r |= b << q;
					break;
				}
			}
			if(i == 9){
				r |= 0x8000000000000000ull;
			}
			*l = r;
			return i + 1;
		}
	};


}}}


#endif