/*
   A function that is obtained by combining two other functions (either by
   addition, multiplication, division or composition)

      addition:          f(x) = f1(x)+f2(x)
      multiplication:    f(x) = f1(x)*f2(x)
      division:          f(x) = f1(x)/f2(x)      f(A) = f2(A)\f1(A)
      composition:       f(x) = f2(f1(x))

   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   SLEPc - Scalable Library for Eigenvalue Problem Computations
   Copyright (c) 2002-2015, Universitat Politecnica de Valencia, Spain

   This file is part of SLEPc.

   SLEPc is free software: you can redistribute it and/or modify it under  the
   terms of version 3 of the GNU Lesser General Public License as published by
   the Free Software Foundation.

   SLEPc  is  distributed in the hope that it will be useful, but WITHOUT  ANY
   WARRANTY;  without even the implied warranty of MERCHANTABILITY or  FITNESS
   FOR  A  PARTICULAR PURPOSE. See the GNU Lesser General Public  License  for
   more details.

   You  should have received a copy of the GNU Lesser General  Public  License
   along with SLEPc. If not, see <http://www.gnu.org/licenses/>.
   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/

#include <slepc/private/fnimpl.h>      /*I "slepcfn.h" I*/
#include <slepcblaslapack.h>

typedef struct {
  FN            f1,f2;    /* functions */
  FNCombineType comb;     /* how the functions are combined */
} FN_COMBINE;

#undef __FUNCT__
#define __FUNCT__ "FNEvaluateFunction_Combine"
PetscErrorCode FNEvaluateFunction_Combine(FN fn,PetscScalar x,PetscScalar *y)
{
  PetscErrorCode ierr;
  FN_COMBINE     *ctx = (FN_COMBINE*)fn->data;
  PetscScalar    a,b;

  PetscFunctionBegin;
  ierr = FNEvaluateFunction(ctx->f1,x,&a);CHKERRQ(ierr);
  switch (ctx->comb) {
    case FN_COMBINE_ADD:
      ierr = FNEvaluateFunction(ctx->f2,x,&b);CHKERRQ(ierr);
      *y = a+b;
      break;
    case FN_COMBINE_MULTIPLY:
      ierr = FNEvaluateFunction(ctx->f2,x,&b);CHKERRQ(ierr);
      *y = a*b;
      break;
    case FN_COMBINE_DIVIDE:
      ierr = FNEvaluateFunction(ctx->f2,x,&b);CHKERRQ(ierr);
      *y = a/b;
      break;
    case FN_COMBINE_COMPOSE:
      ierr = FNEvaluateFunction(ctx->f2,a,y);CHKERRQ(ierr);
      break;
  }
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "FNEvaluateDerivative_Combine"
PetscErrorCode FNEvaluateDerivative_Combine(FN fn,PetscScalar x,PetscScalar *yp)
{
  PetscErrorCode ierr;
  FN_COMBINE     *ctx = (FN_COMBINE*)fn->data;
  PetscScalar    a,b,ap,bp;

  PetscFunctionBegin;
  switch (ctx->comb) {
    case FN_COMBINE_ADD:
      ierr = FNEvaluateDerivative(ctx->f1,x,&ap);CHKERRQ(ierr);
      ierr = FNEvaluateDerivative(ctx->f2,x,&bp);CHKERRQ(ierr);
      *yp = ap+bp;
      break;
    case FN_COMBINE_MULTIPLY:
      ierr = FNEvaluateDerivative(ctx->f1,x,&ap);CHKERRQ(ierr);
      ierr = FNEvaluateDerivative(ctx->f2,x,&bp);CHKERRQ(ierr);
      ierr = FNEvaluateFunction(ctx->f1,x,&a);CHKERRQ(ierr);
      ierr = FNEvaluateFunction(ctx->f2,x,&b);CHKERRQ(ierr);
      *yp = ap*b+a*bp;
      break;
    case FN_COMBINE_DIVIDE:
      ierr = FNEvaluateDerivative(ctx->f1,x,&ap);CHKERRQ(ierr);
      ierr = FNEvaluateDerivative(ctx->f2,x,&bp);CHKERRQ(ierr);
      ierr = FNEvaluateFunction(ctx->f1,x,&a);CHKERRQ(ierr);
      ierr = FNEvaluateFunction(ctx->f2,x,&b);CHKERRQ(ierr);
      *yp = (ap*b-a*bp)/(b*b);
      break;
    case FN_COMBINE_COMPOSE:
      ierr = FNEvaluateFunction(ctx->f1,x,&a);CHKERRQ(ierr);
      ierr = FNEvaluateDerivative(ctx->f1,x,&ap);CHKERRQ(ierr);
      ierr = FNEvaluateDerivative(ctx->f2,a,yp);CHKERRQ(ierr);
      *yp *= ap;
      break;
  }
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "FNEvaluateFunctionMat_Combine"
PetscErrorCode FNEvaluateFunctionMat_Combine(FN fn,Mat A,Mat B)
{
#if defined(PETSC_MISSING_LAPACK_GESV)
  PetscFunctionBegin;
  SETERRQ(PETSC_COMM_SELF,PETSC_ERR_SUP,"GESV - Lapack routines are unavailable");
#else
  PetscErrorCode ierr;
  FN_COMBINE     *ctx = (FN_COMBINE*)fn->data;
  PetscScalar    *Aa,*Ba,*Wa,*Za,one=1.0,zero=0.0;
  PetscBLASInt   n,ld,ld2,inc=1,*ipiv,info;
  PetscInt       m;
  Mat            W,Z;

  PetscFunctionBegin;
  ierr = MatDuplicate(A,MAT_DO_NOT_COPY_VALUES,&W);CHKERRQ(ierr);
  ierr = MatDenseGetArray(A,&Aa);CHKERRQ(ierr);
  ierr = MatDenseGetArray(B,&Ba);CHKERRQ(ierr);
  ierr = MatDenseGetArray(W,&Wa);CHKERRQ(ierr);
  ierr = MatGetSize(A,&m,NULL);CHKERRQ(ierr);
  ierr = PetscBLASIntCast(m,&n);CHKERRQ(ierr);
  ld  = n;
  ld2 = ld*ld;

  switch (ctx->comb) {
    case FN_COMBINE_ADD:
      ierr = FNEvaluateFunctionMat(ctx->f1,A,W);CHKERRQ(ierr);
      ierr = FNEvaluateFunctionMat(ctx->f2,A,B);CHKERRQ(ierr);
      PetscStackCallBLAS("BLASaxpy",BLASaxpy_(&ld2,&one,Wa,&inc,Ba,&inc));
      break;
    case FN_COMBINE_MULTIPLY:
      ierr = MatDuplicate(A,MAT_DO_NOT_COPY_VALUES,&Z);CHKERRQ(ierr);
      ierr = MatDenseGetArray(Z,&Za);CHKERRQ(ierr);
      ierr = FNEvaluateFunctionMat(ctx->f1,A,W);CHKERRQ(ierr);
      ierr = FNEvaluateFunctionMat(ctx->f2,A,Z);CHKERRQ(ierr);
      PetscStackCallBLAS("BLASgemm",BLASgemm_("N","N",&n,&n,&n,&one,Wa,&ld,Za,&ld,&zero,Ba,&ld));
      ierr = MatDenseRestoreArray(Z,&Za);CHKERRQ(ierr);
      ierr = MatDestroy(&Z);CHKERRQ(ierr);
      break;
    case FN_COMBINE_DIVIDE:
      ierr = FNEvaluateFunctionMat(ctx->f1,A,B);CHKERRQ(ierr);
      ierr = FNEvaluateFunctionMat(ctx->f2,A,W);CHKERRQ(ierr);
      ierr = PetscMalloc1(ld,&ipiv);CHKERRQ(ierr);
      PetscStackCallBLAS("LAPACKgesv",LAPACKgesv_(&n,&n,Wa,&ld,ipiv,Ba,&ld,&info));
      if (info) SETERRQ1(PETSC_COMM_SELF,PETSC_ERR_LIB,"Error in Lapack xGESV %d",info);
      ierr = PetscFree(ipiv);CHKERRQ(ierr);
      break;
    case FN_COMBINE_COMPOSE:
      ierr = FNEvaluateFunctionMat(ctx->f1,A,W);CHKERRQ(ierr);
      ierr = FNEvaluateFunctionMat(ctx->f2,W,B);CHKERRQ(ierr);
      break;
  }

  ierr = MatDenseRestoreArray(A,&Aa);CHKERRQ(ierr);
  ierr = MatDenseRestoreArray(B,&Ba);CHKERRQ(ierr);
  ierr = MatDenseRestoreArray(W,&Wa);CHKERRQ(ierr);
  ierr = MatDestroy(&W);CHKERRQ(ierr);
  PetscFunctionReturn(0);
#endif
}

#undef __FUNCT__
#define __FUNCT__ "FNView_Combine"
PetscErrorCode FNView_Combine(FN fn,PetscViewer viewer)
{
  PetscErrorCode ierr;
  FN_COMBINE     *ctx = (FN_COMBINE*)fn->data;
  PetscBool      isascii;

  PetscFunctionBegin;
  ierr = PetscObjectTypeCompare((PetscObject)viewer,PETSCVIEWERASCII,&isascii);CHKERRQ(ierr);
  if (isascii) {
    switch (ctx->comb) {
      case FN_COMBINE_ADD:
        ierr = PetscViewerASCIIPrintf(viewer,"  Two added functions f1+f2\n");CHKERRQ(ierr);
        break;
      case FN_COMBINE_MULTIPLY:
        ierr = PetscViewerASCIIPrintf(viewer,"  Two multiplied functions f1*f2\n");CHKERRQ(ierr);
        break;
      case FN_COMBINE_DIVIDE:
        ierr = PetscViewerASCIIPrintf(viewer,"  A quotient of two functions f1/f2\n");CHKERRQ(ierr);
        break;
      case FN_COMBINE_COMPOSE:
        ierr = PetscViewerASCIIPrintf(viewer,"  Two composed functions f2(f1(.))\n");CHKERRQ(ierr);
        break;
    }
    ierr = PetscViewerASCIIPushTab(viewer);CHKERRQ(ierr);
    ierr = FNView(ctx->f1,viewer);CHKERRQ(ierr);
    ierr = FNView(ctx->f2,viewer);CHKERRQ(ierr);
    ierr = PetscViewerASCIIPopTab(viewer);CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "FNCombineSetChildren_Combine"
static PetscErrorCode FNCombineSetChildren_Combine(FN fn,FNCombineType comb,FN f1,FN f2)
{
  PetscErrorCode ierr;
  FN_COMBINE     *ctx = (FN_COMBINE*)fn->data;

  PetscFunctionBegin;
  ctx->comb = comb;
  ierr = PetscObjectReference((PetscObject)f1);CHKERRQ(ierr);
  ierr = FNDestroy(&ctx->f1);CHKERRQ(ierr);
  ctx->f1 = f1;
  ierr = PetscLogObjectParent((PetscObject)fn,(PetscObject)ctx->f1);CHKERRQ(ierr);
  ierr = PetscObjectReference((PetscObject)f2);CHKERRQ(ierr);
  ierr = FNDestroy(&ctx->f2);CHKERRQ(ierr);
  ctx->f2 = f2;
  ierr = PetscLogObjectParent((PetscObject)fn,(PetscObject)ctx->f2);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "FNCombineSetChildren"
/*@
   FNCombineSetChildren - Sets the two child functions that constitute this
   combined function, and the way they must be combined.

   Logically Collective on FN

   Input Parameters:
+  fn   - the math function context
.  comb - how to combine the functions (addition, multiplication, division or composition)
.  f1   - first function
-  f2   - second function

   Level: intermediate

.seealso: FNCombineGetChildren()
@*/
PetscErrorCode FNCombineSetChildren(FN fn,FNCombineType comb,FN f1,FN f2)
{
  PetscErrorCode ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(fn,FN_CLASSID,1);
  PetscValidLogicalCollectiveEnum(fn,comb,2);
  PetscValidHeaderSpecific(f1,FN_CLASSID,3);
  PetscValidHeaderSpecific(f2,FN_CLASSID,4);
  ierr = PetscTryMethod(fn,"FNCombineSetChildren_C",(FN,FNCombineType,FN,FN),(fn,comb,f1,f2));CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "FNCombineGetChildren_Combine"
static PetscErrorCode FNCombineGetChildren_Combine(FN fn,FNCombineType *comb,FN *f1,FN *f2)
{
  PetscErrorCode ierr;
  FN_COMBINE     *ctx = (FN_COMBINE*)fn->data;

  PetscFunctionBegin;
  if (comb) *comb = ctx->comb;
  if (f1) {
    if (!ctx->f1) {
      ierr = FNCreate(PetscObjectComm((PetscObject)fn),&ctx->f1);CHKERRQ(ierr);
      ierr = PetscLogObjectParent((PetscObject)fn,(PetscObject)ctx->f1);CHKERRQ(ierr);
    }
    *f1 = ctx->f1;
  }
  if (f2) {
    if (!ctx->f2) {
      ierr = FNCreate(PetscObjectComm((PetscObject)fn),&ctx->f2);CHKERRQ(ierr);
      ierr = PetscLogObjectParent((PetscObject)fn,(PetscObject)ctx->f2);CHKERRQ(ierr);
    }
    *f2 = ctx->f2;
  }
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "FNCombineGetChildren"
/*@
   FNCombineGetChildren - Gets the two child functions that constitute this
   combined function, and the way they are combined.

   Not Collective

   Input Parameter:
.  fn   - the math function context

   Output Parameters:
-  comb - how to combine the functions (addition, multiplication, division or composition)
.  f1   - first function
-  f2   - second function

   Level: intermediate

.seealso: FNCombineSetChildren()
@*/
PetscErrorCode FNCombineGetChildren(FN fn,FNCombineType *comb,FN *f1,FN *f2)
{
  PetscErrorCode ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(fn,FN_CLASSID,1);
  ierr = PetscTryMethod(fn,"FNCombineGetChildren_C",(FN,FNCombineType*,FN*,FN*),(fn,comb,f1,f2));CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "FNDuplicate_Combine"
PetscErrorCode FNDuplicate_Combine(FN fn,MPI_Comm comm,FN *newfn)
{
  PetscErrorCode ierr;
  FN_COMBINE     *ctx = (FN_COMBINE*)fn->data,*ctx2;

  PetscFunctionBegin;
  ierr = PetscNewLog(*newfn,&ctx2);CHKERRQ(ierr);
  (*newfn)->data = (void*)ctx2;
  ctx2->comb = ctx->comb;
  ierr = FNDuplicate(ctx->f1,comm,&ctx2->f1);CHKERRQ(ierr);
  ierr = FNDuplicate(ctx->f2,comm,&ctx2->f2);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "FNDestroy_Combine"
PetscErrorCode FNDestroy_Combine(FN fn)
{
  PetscErrorCode ierr;
  FN_COMBINE     *ctx = (FN_COMBINE*)fn->data;

  PetscFunctionBegin;
  ierr = FNDestroy(&ctx->f1);CHKERRQ(ierr);
  ierr = FNDestroy(&ctx->f2);CHKERRQ(ierr);
  ierr = PetscFree(fn->data);CHKERRQ(ierr);
  ierr = PetscObjectComposeFunction((PetscObject)fn,"FNCombineSetChildren_C",NULL);CHKERRQ(ierr);
  ierr = PetscObjectComposeFunction((PetscObject)fn,"FNCombineGetChildren_C",NULL);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "FNCreate_Combine"
PETSC_EXTERN PetscErrorCode FNCreate_Combine(FN fn)
{
  PetscErrorCode ierr;
  FN_COMBINE     *ctx;

  PetscFunctionBegin;
  ierr = PetscNewLog(fn,&ctx);CHKERRQ(ierr);
  fn->data = (void*)ctx;

  fn->ops->evaluatefunction    = FNEvaluateFunction_Combine;
  fn->ops->evaluatederivative  = FNEvaluateDerivative_Combine;
  fn->ops->evaluatefunctionmat = FNEvaluateFunctionMat_Combine;
  fn->ops->view                = FNView_Combine;
  fn->ops->duplicate           = FNDuplicate_Combine;
  fn->ops->destroy             = FNDestroy_Combine;
  ierr = PetscObjectComposeFunction((PetscObject)fn,"FNCombineSetChildren_C",FNCombineSetChildren_Combine);CHKERRQ(ierr);
  ierr = PetscObjectComposeFunction((PetscObject)fn,"FNCombineGetChildren_C",FNCombineGetChildren_Combine);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

