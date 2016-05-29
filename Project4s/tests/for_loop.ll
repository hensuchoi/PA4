; ModuleID = 'for_loop.bc'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-redhat-linux-gnu"

@v = global float 0.000000e+00
@a = global i32 0

define float @fortest() {
fortest:
  %i = alloca i32
  %sum = alloca float
  %v = load float* @v
  %v1 = load float* @v
  %v2 = load float* @v
  store float %v2, float* %sum
  %v3 = load float* @v
  store i32 0, i32* %i
  %i4 = load i32* %i
  %a = load i32* @a
  %a5 = load i32* @a
  %i6 = load i32* %i
  %0 = icmp slt i32 %i6, %a5
  br i1 %0, label %body, label %footer

body7:                                            ; No predecessors!
  %sum9 = load float* %sum
  %sum10 = load float* %sum
  %sum11 = load float* %sum
  %1 = fmul float %sum11, 2.000000e+00
  store float %1, float* %sum
  br label %step

footer:                                           ; preds = %fortest
  %i14 = load i32* %i
  %i15 = load i32* %i
  %i16 = load i32* %i
  %2 = add i32 %i16, 1
  store i32 %2, i32* %i
  %i17 = load i32* %i
  %a18 = load i32* @a
  %a19 = load i32* @a
  %i20 = load i32* %i
  %3 = icmp slt i32 %i20, %a19
  br i1 %3, label %body22, label %footer21

body:                                             ; preds = %fortest
  %sum25 = load float* %sum
  ret float %sum25

body8:                                            ; No predecessors!

step12:                                           ; No predecessors!

step:                                             ; preds = %body7

step13:                                           ; No predecessors!

footer23:                                         ; No predecessors!

footer21:                                         ; preds = %footer

body22:                                           ; preds = %footer

footer24:                                         ; No predecessors!
}
