// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: response_example.proto

#ifndef PROTOBUF_response_5fexample_2eproto__INCLUDED
#define PROTOBUF_response_5fexample_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3002000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3002000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
class request_example_res_t;
class request_example_res_tDefaultTypeInternal;
extern request_example_res_tDefaultTypeInternal _request_example_res_t_default_instance_;

namespace protobuf_response_5fexample_2eproto {
// Internal implementation detail -- do not call these.
struct TableStruct {
  static const ::google::protobuf::uint32 offsets[];
  static void InitDefaultsImpl();
  static void Shutdown();
};
void AddDescriptors();
void InitDefaults();
}  // namespace protobuf_response_5fexample_2eproto

// ===================================================================

class request_example_res_t : public ::google::protobuf::Message   {
 public:
  request_example_res_t();
  virtual ~request_example_res_t();

  request_example_res_t(const request_example_res_t& from);

  inline request_example_res_t& operator=(const request_example_res_t& from) {
    CopyFrom(from);
    return *this;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const request_example_res_t& default_instance();

  static inline const request_example_res_t* internal_default_instance() {
    return reinterpret_cast<const request_example_res_t*>(
               &_request_example_res_t_default_instance_);
  }

  void Swap(request_example_res_t* other);

  // implements Message ----------------------------------------------

  inline request_example_res_t* New() const PROTOBUF_FINAL { return New(NULL); }

  request_example_res_t* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CopyFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void MergeFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void CopyFrom(const request_example_res_t& from);
  void MergeFrom(const request_example_res_t& from);
  void Clear() PROTOBUF_FINAL;
  bool IsInitialized() const PROTOBUF_FINAL;

  size_t ByteSizeLong() const PROTOBUF_FINAL;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) PROTOBUF_FINAL;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const PROTOBUF_FINAL;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const PROTOBUF_FINAL;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output)
      const PROTOBUF_FINAL {
    return InternalSerializeWithCachedSizesToArray(
        ::google::protobuf::io::CodedOutputStream::IsDefaultSerializationDeterministic(), output);
  }
  int GetCachedSize() const PROTOBUF_FINAL { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const PROTOBUF_FINAL;
  void InternalSwap(request_example_res_t* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const PROTOBUF_FINAL;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // string error_info = 2;
  void clear_error_info();
  static const int kErrorInfoFieldNumber = 2;
  const ::std::string& error_info() const;
  void set_error_info(const ::std::string& value);
  #if LANG_CXX11
  void set_error_info(::std::string&& value);
  #endif
  void set_error_info(const char* value);
  void set_error_info(const char* value, size_t size);
  ::std::string* mutable_error_info();
  ::std::string* release_error_info();
  void set_allocated_error_info(::std::string* error_info);

  // string return_data = 3;
  void clear_return_data();
  static const int kReturnDataFieldNumber = 3;
  const ::std::string& return_data() const;
  void set_return_data(const ::std::string& value);
  #if LANG_CXX11
  void set_return_data(::std::string&& value);
  #endif
  void set_return_data(const char* value);
  void set_return_data(const char* value, size_t size);
  ::std::string* mutable_return_data();
  ::std::string* release_return_data();
  void set_allocated_return_data(::std::string* return_data);

  // int32 error_code = 1;
  void clear_error_code();
  static const int kErrorCodeFieldNumber = 1;
  ::google::protobuf::int32 error_code() const;
  void set_error_code(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:request_example_res_t)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::ArenaStringPtr error_info_;
  ::google::protobuf::internal::ArenaStringPtr return_data_;
  ::google::protobuf::int32 error_code_;
  mutable int _cached_size_;
  friend struct  protobuf_response_5fexample_2eproto::TableStruct;
};
// ===================================================================


// ===================================================================

#if !PROTOBUF_INLINE_NOT_IN_HEADERS
// request_example_res_t

// int32 error_code = 1;
inline void request_example_res_t::clear_error_code() {
  error_code_ = 0;
}
inline ::google::protobuf::int32 request_example_res_t::error_code() const {
  // @@protoc_insertion_point(field_get:request_example_res_t.error_code)
  return error_code_;
}
inline void request_example_res_t::set_error_code(::google::protobuf::int32 value) {
  
  error_code_ = value;
  // @@protoc_insertion_point(field_set:request_example_res_t.error_code)
}

// string error_info = 2;
inline void request_example_res_t::clear_error_info() {
  error_info_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& request_example_res_t::error_info() const {
  // @@protoc_insertion_point(field_get:request_example_res_t.error_info)
  return error_info_.GetNoArena();
}
inline void request_example_res_t::set_error_info(const ::std::string& value) {
  
  error_info_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:request_example_res_t.error_info)
}
#if LANG_CXX11
inline void request_example_res_t::set_error_info(::std::string&& value) {
  
  error_info_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:request_example_res_t.error_info)
}
#endif
inline void request_example_res_t::set_error_info(const char* value) {
  
  error_info_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:request_example_res_t.error_info)
}
inline void request_example_res_t::set_error_info(const char* value, size_t size) {
  
  error_info_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:request_example_res_t.error_info)
}
inline ::std::string* request_example_res_t::mutable_error_info() {
  
  // @@protoc_insertion_point(field_mutable:request_example_res_t.error_info)
  return error_info_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* request_example_res_t::release_error_info() {
  // @@protoc_insertion_point(field_release:request_example_res_t.error_info)
  
  return error_info_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void request_example_res_t::set_allocated_error_info(::std::string* error_info) {
  if (error_info != NULL) {
    
  } else {
    
  }
  error_info_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), error_info);
  // @@protoc_insertion_point(field_set_allocated:request_example_res_t.error_info)
}

// string return_data = 3;
inline void request_example_res_t::clear_return_data() {
  return_data_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& request_example_res_t::return_data() const {
  // @@protoc_insertion_point(field_get:request_example_res_t.return_data)
  return return_data_.GetNoArena();
}
inline void request_example_res_t::set_return_data(const ::std::string& value) {
  
  return_data_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:request_example_res_t.return_data)
}
#if LANG_CXX11
inline void request_example_res_t::set_return_data(::std::string&& value) {
  
  return_data_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:request_example_res_t.return_data)
}
#endif
inline void request_example_res_t::set_return_data(const char* value) {
  
  return_data_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:request_example_res_t.return_data)
}
inline void request_example_res_t::set_return_data(const char* value, size_t size) {
  
  return_data_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:request_example_res_t.return_data)
}
inline ::std::string* request_example_res_t::mutable_return_data() {
  
  // @@protoc_insertion_point(field_mutable:request_example_res_t.return_data)
  return return_data_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* request_example_res_t::release_return_data() {
  // @@protoc_insertion_point(field_release:request_example_res_t.return_data)
  
  return return_data_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void request_example_res_t::set_allocated_return_data(::std::string* return_data) {
  if (return_data != NULL) {
    
  } else {
    
  }
  return_data_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), return_data);
  // @@protoc_insertion_point(field_set_allocated:request_example_res_t.return_data)
}

#endif  // !PROTOBUF_INLINE_NOT_IN_HEADERS

// @@protoc_insertion_point(namespace_scope)


// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_response_5fexample_2eproto__INCLUDED