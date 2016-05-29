; ModuleID = 'if_nested.bc'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-redhat-linux-gnu"

@v = global float 0.000000e+00
@arg = global float 0.000000e+00

define float @ifnested() {
ifnested:
  %f = alloca float
  %arg = load float* @arg
  %arg1 = load float* @arg
  %0 = fcmp ogt float %arg1, 1.000000e+00
  br i1 %0, label %then, label %else

then2:                                            ; No predecessors!
  %v = load float* @v
  %v5 = load float* @v
  %1 = fcmp olt float %v5, 5.000000e-01
  br i1 %1, label %then6, label %else4

then:                                             ; preds = %ifnested
  %v9 = load float* @v
  %v10 = load float* @v
  %v11 = load float* @v
  store float %v11, float* %f
  %v12 = load float* @v
  br label %if_footer15

then3:                                            ; No predecessors!
  store float 5.000000e+00, float* %f
  br label %if_footer16

then7:                                            ; No predecessors!
  br label %if_footer

then6:                                            ; preds = %then2
  br label %if_footer22

then8:                                            ; No predecessors!
  %arg23 = load float* @arg
  %arg24 = load float* @arg
  %arg25 = load float* @arg
  store float %arg25, float* %f
  %arg26 = load float* @arg
  br label %if_footer27

else4:                                            ; preds = %then2
  br label %if_footer21

if_footer17:                                      ; No predecessors!
  %f30 = load float* %f
  ret float %f30

then13:                                           ; No predecessors!

then14:                                           ; No predecessors!

if_footer:                                        ; preds = %then7

if_footer15:                                      ; preds = %then

if_footer16:                                      ; preds = %then3

if_footer18:                                      ; No predecessors!

else:                                             ; preds = %ifnested

if_footer28:                                      ; No predecessors!

then19:                                           ; No predecessors!

then20:                                           ; No predecessors!

if_footer21:                                      ; preds = %else4

if_footer22:                                      ; preds = %then6

if_footer27:                                      ; preds = %then8

if_footer29:                                      ; No predecessors!
}
