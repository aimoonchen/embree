// ======================================================================== //
// Copyright 2009-2019 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#pragma once

#include "../math/math.h"

namespace embree
{
  /* 4-wide SSE integer type */
  template<>
  struct vint<4>
  {
    //ALIGNED_STRUCT_(16);
    
    typedef vboolf4 Bool;
    typedef vint4   Int;
    typedef vfloat4 Float;

    enum  { size = 4 };             // number of SIMD elements
    //union { __m128i v; int i[4]; }; // data
    int v;

    ////////////////////////////////////////////////////////////////////////////////
    /// Constructors, Assignment & Cast Operators
    ////////////////////////////////////////////////////////////////////////////////
    
    __forceinline vint() {}
    //__forceinline vint(const vint4& a) { v = a.v; }
    //__forceinline vint4& operator =(const vint4& a) { v = a.v; return *this; }

    //__forceinline vint(__m128i a) : v(a) {}
    //__forceinline operator const __m128i&() const { return v; }
    //__forceinline operator       __m128i&()       { return v; }

    __forceinline vint(int a) : v(a) {}
    __forceinline vint(int a, int b, int c, int d)
    {
      const uint lid = __spirv_BuiltInSubgroupLocalInvocationId;
      if (lid == 0) v = a;
      if (lid == 1) v = b;
      if (lid == 2) v = c;
      if (lid == 3) v = d;
    }

    //__forceinline explicit vint(__m128 a) : v(_mm_cvtps_epi32(a)) {}
    //__forceinline explicit vint(const vboolf4& a) : v(_mm_castps_si128((__m128)a)) {}

    //__forceinline vint(long long a, long long b) : v(_mm_set_epi64x(b,a)) {}

    vint4 fix_upper(int c) const {
      return vint4(__spirv_BuiltInSubgroupLocalInvocationId < 4 ? v : c);
    }
    
    ////////////////////////////////////////////////////////////////////////////////
    /// Constants
    ////////////////////////////////////////////////////////////////////////////////

    __forceinline vint(ZeroTy)        : v(0) {}
    __forceinline vint(OneTy)         : v(1) {}
    __forceinline vint(PosInfTy)      : v(0x7FFFFFFF) {}
    __forceinline vint(NegInfTy)      : v(0x80000000) {}
    //__forceinline vint(StepTy)        : v(_mm_set_epi32(3, 2, 1, 0)) {}
    //__forceinline vint(ReverseStepTy) : v(_mm_set_epi32(0, 1, 2, 3)) {}

    //__forceinline vint(TrueTy)   { v = _mm_cmpeq_epi32(v,v); }
    //__forceinline vint(UndefinedTy) : v(_mm_castps_si128(_mm_undefined_ps())) {}


    ////////////////////////////////////////////////////////////////////////////////
    /// Loads and Stores
    ////////////////////////////////////////////////////////////////////////////////

    static __forceinline vint4 load (const void* a) {
      const uint lid = __spirv_BuiltInSubgroupLocalInvocationId;
      return lid < 4 ? ((int*)a)[lid] : 0.0f;
      //return vint4(__spirv_SubgroupBlockReadINTEL<int>((const __attribute__((ocl_global)) uint32_t*) a));
    }
    static __forceinline vint4 loadu(const void* a) {
      const uint lid = __spirv_BuiltInSubgroupLocalInvocationId;
      return lid < 4 ? ((int*)a)[lid] : 0.0f;
    }

    static __forceinline void store (void* ptr, const vint4& v) {
      const uint lid = __spirv_BuiltInSubgroupLocalInvocationId;
      if (lid < 4) ((int*)ptr)[lid] = v.v;
      //int x = lid < 4 ? v.v : 0.0f;
      //if (lid < 4) __spirv_SubgroupBlockWriteINTEL<uint32_t>((__attribute__((ocl_global)) uint32_t*) ptr, *(uint32_t*)&x);
    }
    static __forceinline void storeu(void* ptr, const vint4& v) {
      const uint lid = __spirv_BuiltInSubgroupLocalInvocationId;
      if (lid < 4) ((int*)ptr)[lid] = v.v;
    }

    //static __forceinline vint4 compact(const vboolf4& mask, vint4 &v) {
    //  return _mm_mask_compress_ps(v, mask, v);
    //}
    //static __forceinline vint4 compact(const vboolf4& mask, vint4 &a, const vint4& b) {
    //  return _mm_mask_compress_ps(a, mask, b);
    //}

    static __forceinline vint4 load (const vboolf4& mask, const void* ptr) {
      if (mask.v) return loadu(ptr); else return vint4(0.0f);
    }
    static __forceinline vint4 loadu(const vboolf4& mask, const void* ptr) {
      if (mask.v) return loadu(ptr); else return vint4(0.0f);
    }

    static __forceinline void store (const vboolf4& mask, void* ptr, const vint4& v) {
      if (mask.v) storeu(ptr,v);
    }
    static __forceinline void storeu(const vboolf4& mask, void* ptr, const vint4& v) {
      if (mask.v) storeu(ptr,v);
    }

    static __forceinline vint4 broadcast(const void* a) {
      return vint4(*(int*)a);
    }

    static __forceinline vint4 load_nt (const int* ptr) {
      return load(ptr);
    }

    static __forceinline void store_nt(void* ptr, const vint4& v) {
      store(ptr,v);
    }

    //static __forceinline vint4 load(const char* ptr) {
    //  return _mm_cvtepi32_ps(_mm_cvtepi8_epi32(_mm_loadu_si128((__m128i*)ptr)));
    //}

    //static __forceinline vint4 load(const unsigned char* ptr) {
    //  return _mm_cvtepi32_ps(_mm_cvtepu8_epi32(_mm_loadu_si128((__m128i*)ptr)));
    //}

    //static __forceinline vint4 load(const short* ptr) {
    //  return _mm_cvtepi32_ps(_mm_cvtepi16_epi32(_mm_loadu_si128((__m128i*)ptr)));
    //}

    //static __forceinline vint4 load(const unsigned short* ptr) {
    //  return _mm_mul_ps(vint4(vint4::load(ptr)),vint4(1.0f/65535.0f));
    //}

    template<int scale = 4>
    static __forceinline vint4 gather(const int* ptr, const vint4& index) {
      return *(int*)((char*)ptr + scale*index.v);
    }

    template<int scale = 4>
      static __forceinline vint4 gather(const vboolf4& mask, const int* ptr, const vint4& index) {
      if (mask.v) return gather<scale>(ptr,index);
      else        return vint4(0.0f);
    }

    template<int scale = 4>
      static __forceinline void scatter(void* ptr, const vint4& index, const vint4& v) {
      *(int*) ((char*)ptr + scale*index.v) = v.v;
    }

    template<int scale = 4>
    static __forceinline void scatter(const vboolf4& mask, void* ptr, const vint4& index, const vint4& v) {
      if (mask.v) scatter<scale>(ptr,index,v);
    }

    static __forceinline void store(const vboolf4& mask, char* ptr, const vint4& ofs, const vint4& v) {
      scatter<1>(mask,ptr,ofs,v);
    }
    static __forceinline void store(const vboolf4& mask, int* ptr, const vint4& ofs, const vint4& v) {
      scatter<4>(mask,ptr,ofs,v);
    }
    
    ////////////////////////////////////////////////////////////////////////////////
    /// Array Access
    ////////////////////////////////////////////////////////////////////////////////

    __forceinline const int operator [](size_t index) const {
      assert(index < 4);
      return __spirv_GroupBroadcast<int>(__spv::Scope::Subgroup, v, index);
    }
    __forceinline int operator [](size_t index) {
      assert(index < 4);
      return __spirv_GroupBroadcast<int>(__spv::Scope::Subgroup, v, index);
    }

    friend __forceinline vint4 select(const vboolf4& m, const vint4& t, const vint4& f) {
      return m.v ? t : f;
    }
  };

  ////////////////////////////////////////////////////////////////////////////////
  /// Unary Operators
  ////////////////////////////////////////////////////////////////////////////////

  //__forceinline vboolf4 asBool(const vint4& a) { return _mm_movepi32_mask(a); }

  __forceinline vint4 operator +(const vint4& a) { return vint4(+a.v); }
  __forceinline vint4 operator -(const vint4& a) { return vint4(-a.v); }
  __forceinline vint4 abs       (const vint4& a) { return vint4(std::abs(a.v)); }

  ////////////////////////////////////////////////////////////////////////////////
  /// Binary Operators
  ////////////////////////////////////////////////////////////////////////////////

  __forceinline vint4 operator +(const vint4& a, const vint4& b) { return vint4(a.v+b.v); }
  __forceinline vint4 operator +(const vint4& a, int          b) { return a + vint4(b); }
  __forceinline vint4 operator +(int          a, const vint4& b) { return vint4(a) + b; }

  __forceinline vint4 operator -(const vint4& a, const vint4& b) { return vint4(a.v+b.v); }
  __forceinline vint4 operator -(const vint4& a, int          b) { return a - vint4(b); }
  __forceinline vint4 operator -(int          a, const vint4& b) { return vint4(a) - b; }

  __forceinline vint4 operator *(const vint4& a, const vint4& b) { return vint4(a.v*b.v); }
  __forceinline vint4 operator *(const vint4& a, int          b) { return a * vint4(b); }
  __forceinline vint4 operator *(int          a, const vint4& b) { return vint4(a) * b; }

  __forceinline vint4 operator &(const vint4& a, const vint4& b) { return vint4(a.v & b.v); }
  __forceinline vint4 operator &(const vint4& a, int          b) { return a & vint4(b); }
  __forceinline vint4 operator &(int          a, const vint4& b) { return vint4(a) & b; }

  __forceinline vint4 operator |(const vint4& a, const vint4& b) { return vint4(a.v | b.v); }
  __forceinline vint4 operator |(const vint4& a, int          b) { return a | vint4(b); }
  __forceinline vint4 operator |(int          a, const vint4& b) { return vint4(a) | b; }

  __forceinline vint4 operator ^(const vint4& a, const vint4& b) { return vint4(a.v ^ b.v); }
  __forceinline vint4 operator ^(const vint4& a, int          b) { return a ^ vint4(b); }
  __forceinline vint4 operator ^(int          a, const vint4& b) { return vint4(a) ^ b; }

  __forceinline vint4 operator <<(const vint4& a, int n) { return vint4(a.v << n); }
  __forceinline vint4 operator >>(const vint4& a, int n) { return vint4(a.v >> n); }

  __forceinline vint4 sll (const vint4& a, int b) { return vint4(a.v << b); }
  __forceinline vint4 sra (const vint4& a, int b) { return vint4(a.v >> b); }
  __forceinline vint4 srl (const vint4& a, int b) { return vint4(unsigned(a.v) >> b); }
  
  ////////////////////////////////////////////////////////////////////////////////
  /// Assignment Operators
  ////////////////////////////////////////////////////////////////////////////////

  __forceinline vint4& operator +=(vint4& a, const vint4& b) { return a = a + b; }
  __forceinline vint4& operator +=(vint4& a, int          b) { return a = a + b; }
  
  __forceinline vint4& operator -=(vint4& a, const vint4& b) { return a = a - b; }
  __forceinline vint4& operator -=(vint4& a, int          b) { return a = a - b; }

#if defined(__SSE4_1__)
  __forceinline vint4& operator *=(vint4& a, const vint4& b) { return a = a * b; }
  __forceinline vint4& operator *=(vint4& a, int          b) { return a = a * b; }
#endif
  
  __forceinline vint4& operator &=(vint4& a, const vint4& b) { return a = a & b; }
  __forceinline vint4& operator &=(vint4& a, int          b) { return a = a & b; }
  
  __forceinline vint4& operator |=(vint4& a, const vint4& b) { return a = a | b; }
  __forceinline vint4& operator |=(vint4& a, int          b) { return a = a | b; }
  
  __forceinline vint4& operator <<=(vint4& a, int b) { return a = a << b; }
  __forceinline vint4& operator >>=(vint4& a, int b) { return a = a >> b; }

  ////////////////////////////////////////////////////////////////////////////////
  /// Comparison Operators + Select
  ////////////////////////////////////////////////////////////////////////////////

  __forceinline vboolf4 operator ==(const vint4& a, const vint4& b) { return vboolf4(a.v == b.v); }
  __forceinline vboolf4 operator !=(const vint4& a, const vint4& b) { return vboolf4(a.v != b.v); }
  __forceinline vboolf4 operator < (const vint4& a, const vint4& b) { return vboolf4(a.v <  b.v); }
  __forceinline vboolf4 operator >=(const vint4& a, const vint4& b) { return vboolf4(a.v >= b.v); }
  __forceinline vboolf4 operator > (const vint4& a, const vint4& b) { return vboolf4(a.v >  b.v); }
  __forceinline vboolf4 operator <=(const vint4& a, const vint4& b) { return vboolf4(a.v <= b.v); }

  __forceinline vboolf4 operator ==(const vint4& a, int          b) { return a == vint4(b); }
  __forceinline vboolf4 operator ==(int          a, const vint4& b) { return vint4(a) == b; }

  __forceinline vboolf4 operator !=(const vint4& a, int          b) { return a != vint4(b); }
  __forceinline vboolf4 operator !=(int          a, const vint4& b) { return vint4(a) != b; }

  __forceinline vboolf4 operator < (const vint4& a, int          b) { return a <  vint4(b); }
  __forceinline vboolf4 operator < (int          a, const vint4& b) { return vint4(a) <  b; }

  __forceinline vboolf4 operator >=(const vint4& a, int          b) { return a >= vint4(b); }
  __forceinline vboolf4 operator >=(int          a, const vint4& b) { return vint4(a) >= b; }

  __forceinline vboolf4 operator > (const vint4& a, int          b) { return a >  vint4(b); }
  __forceinline vboolf4 operator > (int          a, const vint4& b) { return vint4(a) >  b; }

  __forceinline vboolf4 operator <=(const vint4& a, int          b) { return a <= vint4(b); }
  __forceinline vboolf4 operator <=(int          a, const vint4& b) { return vint4(a) <= b; }

  __forceinline vboolf4 eq(const vint4& a, const vint4& b) { return a == b; }
  __forceinline vboolf4 ne(const vint4& a, const vint4& b) { return a != b; }
  __forceinline vboolf4 lt(const vint4& a, const vint4& b) { return a <  b; }
  __forceinline vboolf4 ge(const vint4& a, const vint4& b) { return a >= b; }
  __forceinline vboolf4 gt(const vint4& a, const vint4& b) { return a >  b; }
  __forceinline vboolf4 le(const vint4& a, const vint4& b) { return a <= b; }

  //__forceinline vboolf4 eq(const vboolf4& mask, const vint4& a, const vint4& b) { return _mm_mask_cmp_epi32_mask(mask, a, b, _MM_CMPINT_EQ); }
  //__forceinline vboolf4 ne(const vboolf4& mask, const vint4& a, const vint4& b) { return _mm_mask_cmp_epi32_mask(mask, a, b, _MM_CMPINT_NE); }
  //__forceinline vboolf4 lt(const vboolf4& mask, const vint4& a, const vint4& b) { return _mm_mask_cmp_epi32_mask(mask, a, b, _MM_CMPINT_LT); }
  //__forceinline vboolf4 ge(const vboolf4& mask, const vint4& a, const vint4& b) { return _mm_mask_cmp_epi32_mask(mask, a, b, _MM_CMPINT_GE); }
  //__forceinline vboolf4 gt(const vboolf4& mask, const vint4& a, const vint4& b) { return _mm_mask_cmp_epi32_mask(mask, a, b, _MM_CMPINT_GT); }
  //__forceinline vboolf4 le(const vboolf4& mask, const vint4& a, const vint4& b) { return _mm_mask_cmp_epi32_mask(mask, a, b, _MM_CMPINT_LE); }

/*
  template<int mask>
  __forceinline vint4 select(const vint4& t, const vint4& f) {
#if defined(__SSE4_1__) 
    return _mm_castps_si128(_mm_blend_ps(_mm_castsi128_ps(f), _mm_castsi128_ps(t), mask));
#else
    return select(vboolf4(mask), t, f);
#endif    
  }
  */
  
  __forceinline vint4 min(const vint4& a, const vint4& b) { return vint4(cl::sycl::min(a.v,b.v)); }
  __forceinline vint4 max(const vint4& a, const vint4& b) { return vint4(cl::sycl::max(a.v,b.v)); }

  __forceinline vint4 umin(const vint4& a, const vint4& b) { return vint4(cl::sycl::min(unsigned(a.v),unsigned(b.v))); }
  __forceinline vint4 umax(const vint4& a, const vint4& b) { return vint4(cl::sycl::max(unsigned(a.v),unsigned(b.v))); }

  __forceinline vint4 min(const vint4& a, int          b) { return min(a,vint4(b)); }
  __forceinline vint4 min(int          a, const vint4& b) { return min(vint4(a),b); }
  __forceinline vint4 max(const vint4& a, int          b) { return max(a,vint4(b)); }
  __forceinline vint4 max(int          a, const vint4& b) { return max(vint4(a),b); }

  ////////////////////////////////////////////////////////////////////////////////
  // Movement/Shifting/Shuffling Functions
  ////////////////////////////////////////////////////////////////////////////////
/*
  __forceinline vint4 unpacklo(const vint4& a, const vint4& b) { return _mm_castps_si128(_mm_unpacklo_ps(_mm_castsi128_ps(a), _mm_castsi128_ps(b))); }
  __forceinline vint4 unpackhi(const vint4& a, const vint4& b) { return _mm_castps_si128(_mm_unpackhi_ps(_mm_castsi128_ps(a), _mm_castsi128_ps(b))); }
*/

  template<int i0, int i1, int i2, int i3>
  __forceinline vint4 shuffle(const vint4& v) {
    return vint4(__spirv_SubgroupShuffleINTEL(v.v, vint4(i0,i1,i2,i3).v));
  }

/*
  template<int i0, int i1, int i2, int i3>
  __forceinline vint4 shuffle(const vint4& a, const vint4& b) {
    return _mm_castps_si128(_mm_shuffle_ps(_mm_castsi128_ps(a), _mm_castsi128_ps(b), _MM_SHUFFLE(i3, i2, i1, i0)));
  }
*/

  template<int i>
    __forceinline vint4 shuffle(const vint4& v) {
    return vint4(__spirv_GroupBroadcast(__spv::Scope::Subgroup, v.v, i));
  }

  template<int i> __forceinline int extract(const vint4& a) {
    return __spirv_GroupBroadcast(__spv::Scope::Subgroup, a.v, i);
  }
  
  //template<int dst> __forceinline vint4 insert(const vint4& a, const int b) { return _mm_insert_epi32(a, b, dst); }

  __forceinline int toScalar(const vint4& v) { return extract<0>(v); }
  __forceinline size_t toSizeT(const vint4& v) { return toScalar(v); }

/*
  __forceinline vint4 permute(const vint4 &a, const vint4 &index) {
    return  _mm_castps_si128(_mm_permutevar_ps(_mm_castsi128_ps(a),index));
  }
*/
/*  
  template<int i>
  __forceinline vint4 align_shift_right(const vint4& a, const vint4& b) {
    return _mm_alignr_epi32(a, b, i);    
  }  
*/
  ////////////////////////////////////////////////////////////////////////////////
  /// Reductions
  ////////////////////////////////////////////////////////////////////////////////

  __forceinline vint4 vreduce_min(const vint4& v) {
    return cl::sycl::detail::calc<int, __spv::GroupOperation::Reduce>(v.fix_upper(0x7FFFFFFF).v, cl::sycl::intel::minimum<int>());
  }

  __forceinline vint4 vreduce_max(const vint4& v) {
    return cl::sycl::detail::calc<int, __spv::GroupOperation::Reduce>(v.fix_upper(0x80000000).v, cl::sycl::intel::maximum<int>());
  }
  
  __forceinline vint4 vreduce_add(const vint4& v) {
    return cl::sycl::detail::calc<int, __spv::GroupOperation::Reduce>(v.fix_upper(0).v, cl::sycl::intel::plus<int>());
  }

  __forceinline int reduce_min(const vint4& v) { return vreduce_min(v).v; }
  __forceinline int reduce_max(const vint4& v) { return vreduce_max(v).v; }
  __forceinline int reduce_add(const vint4& v) { return vreduce_add(v).v; }

  //__forceinline size_t select_min(const vint4& v) { return bsf(movemask(v == vreduce_min(v))); }
  //__forceinline size_t select_max(const vint4& v) { return bsf(movemask(v == vreduce_max(v))); }

  //__forceinline size_t select_min(const vboolf4& valid, const vint4& v) { const vint4 a = select(valid,v,vint4(pos_inf)); return bsf(movemask(valid & (a == vreduce_min(a)))); }
  //__forceinline size_t select_max(const vboolf4& valid, const vint4& v) { const vint4 a = select(valid,v,vint4(neg_inf)); return bsf(movemask(valid & (a == vreduce_max(a)))); }

  ////////////////////////////////////////////////////////////////////////////////
  /// Sorting networks
  ////////////////////////////////////////////////////////////////////////////////

#if 0
  
  __forceinline vint4 usort_ascending(const vint4& v)
  {
    const vint4 a0 = v;
    const vint4 b0 = shuffle<1,0,3,2>(a0);
    const vint4 c0 = umin(a0,b0);
    const vint4 d0 = umax(a0,b0);
    const vint4 a1 = select<0x5 /* 0b0101 */>(c0,d0);
    const vint4 b1 = shuffle<2,3,0,1>(a1);
    const vint4 c1 = umin(a1,b1);
    const vint4 d1 = umax(a1,b1);
    const vint4 a2 = select<0x3 /* 0b0011 */>(c1,d1);
    const vint4 b2 = shuffle<0,2,1,3>(a2);
    const vint4 c2 = umin(a2,b2);
    const vint4 d2 = umax(a2,b2);
    const vint4 a3 = select<0x2 /* 0b0010 */>(c2,d2);
    return a3;
  }

  __forceinline vint4 usort_descending(const vint4& v)
  {
    const vint4 a0 = v;
    const vint4 b0 = shuffle<1,0,3,2>(a0);
    const vint4 c0 = umax(a0,b0);
    const vint4 d0 = umin(a0,b0);
    const vint4 a1 = select<0x5 /* 0b0101 */>(c0,d0);
    const vint4 b1 = shuffle<2,3,0,1>(a1);
    const vint4 c1 = umax(a1,b1);
    const vint4 d1 = umin(a1,b1);
    const vint4 a2 = select<0x3 /* 0b0011 */>(c1,d1);
    const vint4 b2 = shuffle<0,2,1,3>(a2);
    const vint4 c2 = umax(a2,b2);
    const vint4 d2 = umin(a2,b2);
    const vint4 a3 = select<0x2 /* 0b0010 */>(c2,d2);
    return a3;
  }

#endif
  
  ////////////////////////////////////////////////////////////////////////////////
  /// Output Operators
  ////////////////////////////////////////////////////////////////////////////////

  inline const cl::sycl::stream& operator <<(const cl::sycl::stream& cout, const vint4& a)
  {
    cout << "<";
    for (int i=0; i<4; i++) {
      if (__spirv_BuiltInSubgroupLocalInvocationId == i) {
        cout << a.v;
        if (i != 3) cout << ", ";
      }
    }
    return cout << ">";
  }

  inline std::ostream& operator <<(std::ostream& cout, const vint4& a) {
    return cout;
  }
}

