; ModuleID = 'mainModule'
source_filename = "mainModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%Rectangle = type { %Point, %Point }
%Point = type { i64, i64 }

@0 = private unnamed_addr constant [11 x i8] c"p2: %d %d \00", align 1
@1 = private unnamed_addr constant [46 x i8] c"height: %d, width: %d, area: %d, contains: %d\00", align 1

define i64 @"#Rectangle.$height?Pointer(Class(Rectangle))?"(ptr %0) {
alloca:
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  br label %entry

entry:                                            ; preds = %alloca
  %ptrAddress = load ptr, ptr %this, align 8
  %bottomrightPtr = getelementptr inbounds %Rectangle, ptr %ptrAddress, i32 0, i32 1
  %yPtr = getelementptr inbounds %Point, ptr %bottomrightPtr, i32 0, i32 1
  %fieldRef = load i64, ptr %yPtr, align 8
  %ptrAddress1 = load ptr, ptr %this, align 8
  %topleftPtr = getelementptr inbounds %Rectangle, ptr %ptrAddress1, i32 0, i32 0
  %yPtr2 = getelementptr inbounds %Point, ptr %topleftPtr, i32 0, i32 1
  %fieldRef3 = load i64, ptr %yPtr2, align 8
  %1 = sub i64 %fieldRef, %fieldRef3
  ret i64 %1
}

define i64 @"#Rectangle.$width?Pointer(Class(Rectangle))?"(ptr %0) {
alloca:
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  br label %entry

entry:                                            ; preds = %alloca
  %ptrAddress = load ptr, ptr %this, align 8
  %bottomrightPtr = getelementptr inbounds %Rectangle, ptr %ptrAddress, i32 0, i32 1
  %xPtr = getelementptr inbounds %Point, ptr %bottomrightPtr, i32 0, i32 0
  %fieldRef = load i64, ptr %xPtr, align 8
  %ptrAddress1 = load ptr, ptr %this, align 8
  %topleftPtr = getelementptr inbounds %Rectangle, ptr %ptrAddress1, i32 0, i32 0
  %xPtr2 = getelementptr inbounds %Point, ptr %topleftPtr, i32 0, i32 0
  %fieldRef3 = load i64, ptr %xPtr2, align 8
  %1 = sub i64 %fieldRef, %fieldRef3
  ret i64 %1
}

define i64 @"#Rectangle.$area?Pointer(Class(Rectangle))?"(ptr %0) {
alloca:
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  br label %entry

entry:                                            ; preds = %alloca
  %ptrAddress = load ptr, ptr %this, align 8
  %1 = call i64 @"#Rectangle.$height?Pointer(Class(Rectangle))?"(ptr %ptrAddress)
  %ptrAddress1 = load ptr, ptr %this, align 8
  %2 = call i64 @"#Rectangle.$width?Pointer(Class(Rectangle))?"(ptr %ptrAddress1)
  %3 = mul i64 %1, %2
  ret i64 %3
}

define i1 @"#Rectangle.$contains?Pointer(Class(Rectangle))?Pointer(Class(Point))?"(ptr %0, ptr %1) {
alloca:
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %p = alloca ptr, align 8
  store ptr %1, ptr %p, align 8
  br label %entry

entry:                                            ; preds = %alloca
  %ptrAddress = load ptr, ptr %p, align 8
  %xPtr = getelementptr inbounds %Point, ptr %ptrAddress, i32 0, i32 0
  %fieldRef = load i64, ptr %xPtr, align 8
  %ptrAddress1 = load ptr, ptr %this, align 8
  %topleftPtr = getelementptr inbounds %Rectangle, ptr %ptrAddress1, i32 0, i32 0
  %xPtr2 = getelementptr inbounds %Point, ptr %topleftPtr, i32 0, i32 0
  %fieldRef3 = load i64, ptr %xPtr2, align 8
  %2 = icmp sge i64 %fieldRef, %fieldRef3
  %ptrAddress4 = load ptr, ptr %p, align 8
  %xPtr5 = getelementptr inbounds %Point, ptr %ptrAddress4, i32 0, i32 0
  %fieldRef6 = load i64, ptr %xPtr5, align 8
  %ptrAddress7 = load ptr, ptr %this, align 8
  %bottomrightPtr = getelementptr inbounds %Rectangle, ptr %ptrAddress7, i32 0, i32 1
  %xPtr8 = getelementptr inbounds %Point, ptr %bottomrightPtr, i32 0, i32 0
  %fieldRef9 = load i64, ptr %xPtr8, align 8
  %3 = icmp sle i64 %fieldRef6, %fieldRef9
  %4 = and i1 %2, %3
  %ptrAddress10 = load ptr, ptr %p, align 8
  %yPtr = getelementptr inbounds %Point, ptr %ptrAddress10, i32 0, i32 1
  %fieldRef11 = load i64, ptr %yPtr, align 8
  %ptrAddress12 = load ptr, ptr %this, align 8
  %topleftPtr13 = getelementptr inbounds %Rectangle, ptr %ptrAddress12, i32 0, i32 0
  %yPtr14 = getelementptr inbounds %Point, ptr %topleftPtr13, i32 0, i32 1
  %fieldRef15 = load i64, ptr %yPtr14, align 8
  %5 = icmp sge i64 %fieldRef11, %fieldRef15
  %6 = and i1 %4, %5
  %ptrAddress16 = load ptr, ptr %p, align 8
  %yPtr17 = getelementptr inbounds %Point, ptr %ptrAddress16, i32 0, i32 1
  %fieldRef18 = load i64, ptr %yPtr17, align 8
  %ptrAddress19 = load ptr, ptr %this, align 8
  %bottomrightPtr20 = getelementptr inbounds %Rectangle, ptr %ptrAddress19, i32 0, i32 1
  %yPtr21 = getelementptr inbounds %Point, ptr %bottomrightPtr20, i32 0, i32 1
  %fieldRef22 = load i64, ptr %yPtr21, align 8
  %7 = icmp sle i64 %fieldRef18, %fieldRef22
  %8 = and i1 %6, %7
  ret i1 %8
}

define i64 @main() {
alloca:
  br label %entry

entry:                                            ; preds = %alloca
  %p1 = alloca %Point, align 8
  %structLiteral = alloca %Point, align 8
  %xPtr = getelementptr inbounds %Point, ptr %structLiteral, i32 0, i32 0
  store i64 0, ptr %xPtr, align 8
  %yPtr = getelementptr inbounds %Point, ptr %structLiteral, i32 0, i32 1
  store i64 0, ptr %yPtr, align 8
  %structLiteral1 = load %Point, ptr %structLiteral, align 8
  store %Point %structLiteral1, ptr %p1, align 8
  %p2 = alloca %Point, align 8
  %yPtr2 = getelementptr inbounds %Point, ptr %p2, i32 0, i32 1
  store i64 0, ptr %yPtr2, align 8
  %xPtr3 = getelementptr inbounds %Point, ptr %p2, i32 0, i32 0
  store i64 0, ptr %xPtr3, align 8
  %xPtr4 = getelementptr inbounds %Point, ptr %p1, i32 0, i32 0
  %fieldRef = load i64, ptr %xPtr4, align 8
  %xPtr5 = getelementptr inbounds %Point, ptr %p2, i32 0, i32 0
  %fieldRef6 = load i64, ptr %xPtr5, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @0, i64 %fieldRef, i64 %fieldRef6)
  %xPtr7 = getelementptr inbounds %Point, ptr %p2, i32 0, i32 0
  store volatile i64 10, ptr %xPtr7, align 8
  %loadAssigned = load i64, ptr %xPtr7, align 8
  %yPtr8 = getelementptr inbounds %Point, ptr %p2, i32 0, i32 1
  store volatile i64 10, ptr %yPtr8, align 8
  %loadAssigned9 = load i64, ptr %yPtr8, align 8
  %r = alloca %Rectangle, align 8
  %structLiteral10 = alloca %Rectangle, align 8
  %topleftPtr = getelementptr inbounds %Rectangle, ptr %structLiteral10, i32 0, i32 0
  %p111 = load %Point, ptr %p1, align 8
  store %Point %p111, ptr %topleftPtr, align 8
  %bottomrightPtr = getelementptr inbounds %Rectangle, ptr %structLiteral10, i32 0, i32 1
  %p212 = load %Point, ptr %p2, align 8
  store %Point %p212, ptr %bottomrightPtr, align 8
  %structLiteral13 = load %Rectangle, ptr %structLiteral10, align 8
  store %Rectangle %structLiteral13, ptr %r, align 8
  %height = alloca i64, align 8
  %1 = call i64 @"#Rectangle.$height?Pointer(Class(Rectangle))?"(ptr %r)
  store i64 %1, ptr %height, align 8
  %width = alloca i64, align 8
  %2 = call i64 @"#Rectangle.$width?Pointer(Class(Rectangle))?"(ptr %r)
  store i64 %2, ptr %width, align 8
  %area = alloca i64, align 8
  %3 = call i64 @"#Rectangle.$area?Pointer(Class(Rectangle))?"(ptr %r)
  store i64 %3, ptr %area, align 8
  %p = alloca %Point, align 8
  %structLiteral14 = alloca %Point, align 8
  %xPtr15 = getelementptr inbounds %Point, ptr %structLiteral14, i32 0, i32 0
  store i64 5, ptr %xPtr15, align 8
  %yPtr16 = getelementptr inbounds %Point, ptr %structLiteral14, i32 0, i32 1
  store i64 5, ptr %yPtr16, align 8
  %structLiteral17 = load %Point, ptr %structLiteral14, align 8
  store %Point %structLiteral17, ptr %p, align 8
  %contains = alloca i1, align 1
  %4 = call i1 @"#Rectangle.$contains?Pointer(Class(Rectangle))?Pointer(Class(Point))?"(ptr %r, ptr %p)
  store i1 %4, ptr %contains, align 1
  %height18 = load i64, ptr %height, align 8
  %width19 = load i64, ptr %width, align 8
  %area20 = load i64, ptr %area, align 8
  %contains21 = load i1, ptr %contains, align 1
  %5 = call i32 (ptr, ...) @printf(ptr @1, i64 %height18, i64 %width19, i64 %area20, i1 %contains21)
  ret i64 0
}

declare i32 @printf(ptr, ...)
