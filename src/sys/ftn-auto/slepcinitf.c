#include "petscsys.h"
#include "petscfix.h"
#include "petsc-private/fortranimpl.h"
/* slepcinit.c */
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

#define PetscToPointer(a) (*(long *)(a))
#define PetscFromPointer(a) (long)(a)
#define PetscRmPointer(a)
#endif

#include "slepcsys.h"
#ifdef PETSC_HAVE_FORTRAN_CAPS
#define slepcfinalize_ SLEPCFINALIZE
#elif !defined(PETSC_HAVE_FORTRAN_UNDERSCORE) && !defined(FORTRANDOUBLEUNDERSCORE)
#define slepcfinalize_ slepcfinalize
#endif
#ifdef PETSC_HAVE_FORTRAN_CAPS
#define slepcinitialized_ SLEPCINITIALIZED
#elif !defined(PETSC_HAVE_FORTRAN_UNDERSCORE) && !defined(FORTRANDOUBLEUNDERSCORE)
#define slepcinitialized_ slepcinitialized
#endif


/* Definitions of Fortran Wrapper routines */
#if defined(__cplusplus)
extern "C" {
#endif
PETSC_EXTERN void PETSC_STDCALL  slepcfinalize_(int *__ierr ){
*__ierr = SlepcFinalize();
}
PETSC_EXTERN void PETSC_STDCALL  slepcinitialized_(PetscBool *isInitialized, int *__ierr ){
*__ierr = SlepcInitialized(isInitialized);
}
#if defined(__cplusplus)
}
#endif
