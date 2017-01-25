//===--- NativeConventionSchema.h - R-Value Schema for SwiftCC  -*- C++ -*-===//
//
// This source file is part of the Swift.org open source project
//
// Copyright (c) 2014 - 2016 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See http://swift.org/LICENSE.txt for license information
// See http://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//
//===----------------------------------------------------------------------===//
//
// A schema that describes the explosion of values for passing according to the
// native calling convention.
//
//===----------------------------------------------------------------------===//
#ifndef SWIFT_IRGEN_NATIVECONVENTIONSCHEMA_H
#define SWIFT_IRGEN_NATIVECONVENTIONSCHEMA_H

#include "clang/CodeGen/SwiftCallingConv.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallVector.h"
#include "IRGen.h"
#include "IRGenFunction.h"

namespace swift {
namespace irgen {

class NativeConventionSchema {
  clang::CodeGen::swiftcall::SwiftAggLowering Lowering;
  bool RequiresIndirect;

public:
  using EnumerationCallback =
      clang::CodeGen::swiftcall::SwiftAggLowering::EnumerationCallback;

  NativeConventionSchema(IRGenModule &IGM, const TypeInfo *TI, bool isResult);

  NativeConventionSchema() = delete;
  NativeConventionSchema(const NativeConventionSchema &) = delete;
  NativeConventionSchema &operator=(const NativeConventionSchema&) = delete;

  bool requiresIndirect() const { return RequiresIndirect; }
  bool empty() const { return Lowering.empty(); }

  llvm::Type *getExpandedType(IRGenModule &IGM) const;

  /// The number of components in the schema.
  unsigned size() const;

  void enumerateComponents(EnumerationCallback callback) const {
    Lowering.enumerateComponents(callback);
  }

  /// Map from a non-native explosion to an explosion that follows the native
  /// calling convention's schema.
  Explosion mapIntoNative(IRGenModule &IGM, IRGenFunction &IGF,
                          Explosion &fromNonNative, SILType type) const;

  /// Map form a native explosion that follows the native calling convention's
  /// schema to an non-native explosion whose schema is described by
  /// type.getSchema().
  Explosion mapFromNative(IRGenModule &IGM, IRGenFunction &IGF,
                          Explosion &native, SILType type) const;

  /// Return a pair of structs that can be used to load/store the components of
  /// the native schema from/to the memory representation as defined by the
  /// value's loadable type info.
  /// The second layout is only necessary if there are overlapping components in
  /// the legal type sequence. It contains the non-integer components of
  /// overlapped components of the legal type sequence.
  ///
  /// \p ExpandedTyIndices is a map from the non-array type elements of the
  /// returned struct types (viewed concatenated) to the index in the expanded
  /// type.
  std::pair<llvm::StructType *, llvm::StructType *>
  getCoercionTypes(IRGenModule &IGM,
                   SmallVectorImpl<unsigned> &expandedTyIndicesMap) const;
};


} // end namespace irgen
} // end namespace swift

#endif
