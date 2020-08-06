

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0622 */
/* at Mon Jan 18 22:14:07 2038
 */
/* Compiler settings for MOC_Grid.odl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.01.0622 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */



/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */


#ifndef __MOC_Grid_h_h__
#define __MOC_Grid_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IMOC_Grid_FWD_DEFINED__
#define __IMOC_Grid_FWD_DEFINED__
typedef interface IMOC_Grid IMOC_Grid;

#endif 	/* __IMOC_Grid_FWD_DEFINED__ */


#ifndef __MOC_Grid_FWD_DEFINED__
#define __MOC_Grid_FWD_DEFINED__

#ifdef __cplusplus
typedef class MOC_Grid MOC_Grid;
#else
typedef struct MOC_Grid MOC_Grid;
#endif /* __cplusplus */

#endif 	/* __MOC_Grid_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __MOC_Grid_LIBRARY_DEFINED__
#define __MOC_Grid_LIBRARY_DEFINED__

/* library MOC_Grid */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_MOC_Grid;

#ifndef __IMOC_Grid_DISPINTERFACE_DEFINED__
#define __IMOC_Grid_DISPINTERFACE_DEFINED__

/* dispinterface IMOC_Grid */
/* [uuid] */ 


EXTERN_C const IID DIID_IMOC_Grid;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("64AD481E-9EB6-4090-9F6C-D9AB592DD003")
    IMOC_Grid : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IMOC_GridVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMOC_Grid * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMOC_Grid * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMOC_Grid * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IMOC_Grid * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IMOC_Grid * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IMOC_Grid * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IMOC_Grid * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        END_INTERFACE
    } IMOC_GridVtbl;

    interface IMOC_Grid
    {
        CONST_VTBL struct IMOC_GridVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMOC_Grid_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMOC_Grid_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMOC_Grid_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IMOC_Grid_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IMOC_Grid_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IMOC_Grid_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IMOC_Grid_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IMOC_Grid_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_MOC_Grid;

#ifdef __cplusplus

class DECLSPEC_UUID("1506B334-8117-4B15-A2BA-2C0D344ED59B")
MOC_Grid;
#endif
#endif /* __MOC_Grid_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


