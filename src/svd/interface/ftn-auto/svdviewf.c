#include "petscsys.h"
#include "petscfix.h"
#include "petsc/private/fortranimpl.h"
/* svdview.c */
/* Fortran interface file */

/*
* This file was generated automatically by bfort from the C source
* file.  
 */

#ifdef PETSC_USE_POINTER_CONVERSION
#if defined(__cplusplus)
extern "C" { 
#endif 
extern void *PetscToPointer(void*);
extern int PetscFromPointer(void *);
extern void PetscRmPointer(void*);
#if defined(__cplusplus)
} 
#endif 

#else

#define PetscToPointer(a) (*(PetscFortranAddr *)(a))
#define PetscFromPointer(a) (PetscFortranAddr)(a)
#define PetscRmPointer(a)
#endif

#include "slepcsvd.h"
#ifdef PETSC_HAVE_FORTRAN_CAPS
#define svdreasonviewfromoptions_ SVDREASONVIEWFROMOPTIONS
#elif !defined(PETSC_HAVE_FORTRAN_UNDERSCORE) && !defined(FORTRANDOUBLEUNDERSCORE)
#define svdreasonviewfromoptions_ svdreasonviewfromoptions
#endif
#ifdef PETSC_HAVE_FORTRAN_CAPS
#define svderrorviewfromoptions_ SVDERRORVIEWFROMOPTIONS
#elif !defined(PETSC_HAVE_FORTRAN_UNDERSCORE) && !defined(FORTRANDOUBLEUNDERSCORE)
#define svderrorviewfromoptions_ svderrorviewfromoptions
#endif
#ifdef PETSC_HAVE_FORTRAN_CAPS
#define svdvaluesviewfromoptions_ SVDVALUESVIEWFROMOPTIONS
#elif !defined(PETSC_HAVE_FORTRAN_UNDERSCORE) && !defined(FORTRANDOUBLEUNDERSCORE)
#define svdvaluesviewfromoptions_ svdvaluesviewfromoptions
#endif
#ifdef PETSC_HAVE_FORTRAN_CAPS
#define svdvectorsviewfromoptions_ SVDVECTORSVIEWFROMOPTIONS
#elif !defined(PETSC_HAVE_FORTRAN_UNDERSCORE) && !defined(FORTRANDOUBLEUNDERSCORE)
#define svdvectorsviewfromoptions_ svdvectorsviewfromoptions
#endif


/* Definitions of Fortran Wrapper routines */
#if defined(__cplusplus)
extern "C" {
#endif
PETSC_EXTERN void PETSC_STDCALL  svdreasonviewfromoptions_(SVD *svd, int *__ierr ){
*__ierr = SVDReasonViewFromOptions(*svd);
}
PETSC_EXTERN void PETSC_STDCALL  svderrorviewfromoptions_(SVD *svd, int *__ierr ){
*__ierr = SVDErrorViewFromOptions(*svd);
}
PETSC_EXTERN void PETSC_STDCALL  svdvaluesviewfromoptions_(SVD *svd, int *__ierr ){
*__ierr = SVDValuesViewFromOptions(*svd);
}
PETSC_EXTERN void PETSC_STDCALL  svdvectorsviewfromoptions_(SVD *svd, int *__ierr ){
*__ierr = SVDVectorsViewFromOptions(*svd);
}
#if defined(__cplusplus)
}
#endif
