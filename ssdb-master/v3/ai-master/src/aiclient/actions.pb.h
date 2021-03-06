// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: actions.proto

#ifndef PROTOBUF_actions_2eproto__INCLUDED
#define PROTOBUF_actions_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3001000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3001000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace pb {

// Internal implementation detail -- do not call these.
void protobuf_AddDesc_actions_2eproto();
void protobuf_InitDefaults_actions_2eproto();
void protobuf_AssignDesc_actions_2eproto();
void protobuf_ShutdownFile_actions_2eproto();

class Action;
class ActionList;
class ActionVector;

// ===================================================================

class ActionList : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:pb.ActionList) */ {
 public:
  ActionList();
  virtual ~ActionList();

  ActionList(const ActionList& from);

  inline ActionList& operator=(const ActionList& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _internal_metadata_.unknown_fields();
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return _internal_metadata_.mutable_unknown_fields();
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const ActionList& default_instance();

  static const ActionList* internal_default_instance();

  void Swap(ActionList* other);

  // implements Message ----------------------------------------------

  inline ActionList* New() const { return New(NULL); }

  ActionList* New(::google::protobuf::Arena* arena) const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const ActionList& from);
  void MergeFrom(const ActionList& from);
  void Clear();
  bool IsInitialized() const;

  size_t ByteSizeLong() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const {
    return InternalSerializeWithCachedSizesToArray(false, output);
  }
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  void InternalSwap(ActionList* other);
  void UnsafeMergeFrom(const ActionList& from);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return _internal_metadata_.arena();
  }
  inline void* MaybeArenaPtr() const {
    return _internal_metadata_.raw_arena_ptr();
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // repeated .pb.Action actionitem = 1;
  int actionitem_size() const;
  void clear_actionitem();
  static const int kActionitemFieldNumber = 1;
  const ::pb::Action& actionitem(int index) const;
  ::pb::Action* mutable_actionitem(int index);
  ::pb::Action* add_actionitem();
  ::google::protobuf::RepeatedPtrField< ::pb::Action >*
      mutable_actionitem();
  const ::google::protobuf::RepeatedPtrField< ::pb::Action >&
      actionitem() const;

  // @@protoc_insertion_point(class_scope:pb.ActionList)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::HasBits<1> _has_bits_;
  mutable int _cached_size_;
  ::google::protobuf::RepeatedPtrField< ::pb::Action > actionitem_;
  friend void  protobuf_InitDefaults_actions_2eproto_impl();
  friend void  protobuf_AddDesc_actions_2eproto_impl();
  friend void protobuf_AssignDesc_actions_2eproto();
  friend void protobuf_ShutdownFile_actions_2eproto();

  void InitAsDefaultInstance();
};
extern ::google::protobuf::internal::ExplicitlyConstructed<ActionList> ActionList_default_instance_;

// -------------------------------------------------------------------

class ActionVector : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:pb.ActionVector) */ {
 public:
  ActionVector();
  virtual ~ActionVector();

  ActionVector(const ActionVector& from);

  inline ActionVector& operator=(const ActionVector& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _internal_metadata_.unknown_fields();
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return _internal_metadata_.mutable_unknown_fields();
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const ActionVector& default_instance();

  static const ActionVector* internal_default_instance();

  void Swap(ActionVector* other);

  // implements Message ----------------------------------------------

  inline ActionVector* New() const { return New(NULL); }

  ActionVector* New(::google::protobuf::Arena* arena) const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const ActionVector& from);
  void MergeFrom(const ActionVector& from);
  void Clear();
  bool IsInitialized() const;

  size_t ByteSizeLong() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const {
    return InternalSerializeWithCachedSizesToArray(false, output);
  }
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  void InternalSwap(ActionVector* other);
  void UnsafeMergeFrom(const ActionVector& from);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return _internal_metadata_.arena();
  }
  inline void* MaybeArenaPtr() const {
    return _internal_metadata_.raw_arena_ptr();
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required sint32 x = 1;
  bool has_x() const;
  void clear_x();
  static const int kXFieldNumber = 1;
  ::google::protobuf::int32 x() const;
  void set_x(::google::protobuf::int32 value);

  // required sint32 z = 2;
  bool has_z() const;
  void clear_z();
  static const int kZFieldNumber = 2;
  ::google::protobuf::int32 z() const;
  void set_z(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:pb.ActionVector)
 private:
  inline void set_has_x();
  inline void clear_has_x();
  inline void set_has_z();
  inline void clear_has_z();

  // helper for ByteSizeLong()
  size_t RequiredFieldsByteSizeFallback() const;

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::HasBits<1> _has_bits_;
  mutable int _cached_size_;
  ::google::protobuf::int32 x_;
  ::google::protobuf::int32 z_;
  friend void  protobuf_InitDefaults_actions_2eproto_impl();
  friend void  protobuf_AddDesc_actions_2eproto_impl();
  friend void protobuf_AssignDesc_actions_2eproto();
  friend void protobuf_ShutdownFile_actions_2eproto();

  void InitAsDefaultInstance();
};
extern ::google::protobuf::internal::ExplicitlyConstructed<ActionVector> ActionVector_default_instance_;

// -------------------------------------------------------------------

class Action : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:pb.Action) */ {
 public:
  Action();
  virtual ~Action();

  Action(const Action& from);

  inline Action& operator=(const Action& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _internal_metadata_.unknown_fields();
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return _internal_metadata_.mutable_unknown_fields();
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const Action& default_instance();

  static const Action* internal_default_instance();

  void Swap(Action* other);

  // implements Message ----------------------------------------------

  inline Action* New() const { return New(NULL); }

  Action* New(::google::protobuf::Arena* arena) const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Action& from);
  void MergeFrom(const Action& from);
  void Clear();
  bool IsInitialized() const;

  size_t ByteSizeLong() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const {
    return InternalSerializeWithCachedSizesToArray(false, output);
  }
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  void InternalSwap(Action* other);
  void UnsafeMergeFrom(const Action& from);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return _internal_metadata_.arena();
  }
  inline void* MaybeArenaPtr() const {
    return _internal_metadata_.raw_arena_ptr();
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required string action = 1;
  bool has_action() const;
  void clear_action();
  static const int kActionFieldNumber = 1;
  const ::std::string& action() const;
  void set_action(const ::std::string& value);
  void set_action(const char* value);
  void set_action(const char* value, size_t size);
  ::std::string* mutable_action();
  ::std::string* release_action();
  void set_allocated_action(::std::string* action);

  // required .pb.ActionVector pos = 2;
  bool has_pos() const;
  void clear_pos();
  static const int kPosFieldNumber = 2;
  const ::pb::ActionVector& pos() const;
  ::pb::ActionVector* mutable_pos();
  ::pb::ActionVector* release_pos();
  void set_allocated_pos(::pb::ActionVector* pos);

  // required .pb.ActionVector dir = 3;
  bool has_dir() const;
  void clear_dir();
  static const int kDirFieldNumber = 3;
  const ::pb::ActionVector& dir() const;
  ::pb::ActionVector* mutable_dir();
  ::pb::ActionVector* release_dir();
  void set_allocated_dir(::pb::ActionVector* dir);

  // required uint32 xml_id = 4;
  bool has_xml_id() const;
  void clear_xml_id();
  static const int kXmlIdFieldNumber = 4;
  ::google::protobuf::uint32 xml_id() const;
  void set_xml_id(::google::protobuf::uint32 value);

  // required uint32 job = 5;
  bool has_job() const;
  void clear_job();
  static const int kJobFieldNumber = 5;
  ::google::protobuf::uint32 job() const;
  void set_job(::google::protobuf::uint32 value);

  // @@protoc_insertion_point(class_scope:pb.Action)
 private:
  inline void set_has_action();
  inline void clear_has_action();
  inline void set_has_pos();
  inline void clear_has_pos();
  inline void set_has_dir();
  inline void clear_has_dir();
  inline void set_has_xml_id();
  inline void clear_has_xml_id();
  inline void set_has_job();
  inline void clear_has_job();

  // helper for ByteSizeLong()
  size_t RequiredFieldsByteSizeFallback() const;

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::HasBits<1> _has_bits_;
  mutable int _cached_size_;
  ::google::protobuf::internal::ArenaStringPtr action_;
  ::pb::ActionVector* pos_;
  ::pb::ActionVector* dir_;
  ::google::protobuf::uint32 xml_id_;
  ::google::protobuf::uint32 job_;
  friend void  protobuf_InitDefaults_actions_2eproto_impl();
  friend void  protobuf_AddDesc_actions_2eproto_impl();
  friend void protobuf_AssignDesc_actions_2eproto();
  friend void protobuf_ShutdownFile_actions_2eproto();

  void InitAsDefaultInstance();
};
extern ::google::protobuf::internal::ExplicitlyConstructed<Action> Action_default_instance_;

// ===================================================================


// ===================================================================

#if !PROTOBUF_INLINE_NOT_IN_HEADERS
// ActionList

// repeated .pb.Action actionitem = 1;
inline int ActionList::actionitem_size() const {
  return actionitem_.size();
}
inline void ActionList::clear_actionitem() {
  actionitem_.Clear();
}
inline const ::pb::Action& ActionList::actionitem(int index) const {
  // @@protoc_insertion_point(field_get:pb.ActionList.actionitem)
  return actionitem_.Get(index);
}
inline ::pb::Action* ActionList::mutable_actionitem(int index) {
  // @@protoc_insertion_point(field_mutable:pb.ActionList.actionitem)
  return actionitem_.Mutable(index);
}
inline ::pb::Action* ActionList::add_actionitem() {
  // @@protoc_insertion_point(field_add:pb.ActionList.actionitem)
  return actionitem_.Add();
}
inline ::google::protobuf::RepeatedPtrField< ::pb::Action >*
ActionList::mutable_actionitem() {
  // @@protoc_insertion_point(field_mutable_list:pb.ActionList.actionitem)
  return &actionitem_;
}
inline const ::google::protobuf::RepeatedPtrField< ::pb::Action >&
ActionList::actionitem() const {
  // @@protoc_insertion_point(field_list:pb.ActionList.actionitem)
  return actionitem_;
}

inline const ActionList* ActionList::internal_default_instance() {
  return &ActionList_default_instance_.get();
}
// -------------------------------------------------------------------

// ActionVector

// required sint32 x = 1;
inline bool ActionVector::has_x() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void ActionVector::set_has_x() {
  _has_bits_[0] |= 0x00000001u;
}
inline void ActionVector::clear_has_x() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void ActionVector::clear_x() {
  x_ = 0;
  clear_has_x();
}
inline ::google::protobuf::int32 ActionVector::x() const {
  // @@protoc_insertion_point(field_get:pb.ActionVector.x)
  return x_;
}
inline void ActionVector::set_x(::google::protobuf::int32 value) {
  set_has_x();
  x_ = value;
  // @@protoc_insertion_point(field_set:pb.ActionVector.x)
}

// required sint32 z = 2;
inline bool ActionVector::has_z() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void ActionVector::set_has_z() {
  _has_bits_[0] |= 0x00000002u;
}
inline void ActionVector::clear_has_z() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void ActionVector::clear_z() {
  z_ = 0;
  clear_has_z();
}
inline ::google::protobuf::int32 ActionVector::z() const {
  // @@protoc_insertion_point(field_get:pb.ActionVector.z)
  return z_;
}
inline void ActionVector::set_z(::google::protobuf::int32 value) {
  set_has_z();
  z_ = value;
  // @@protoc_insertion_point(field_set:pb.ActionVector.z)
}

inline const ActionVector* ActionVector::internal_default_instance() {
  return &ActionVector_default_instance_.get();
}
// -------------------------------------------------------------------

// Action

// required string action = 1;
inline bool Action::has_action() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void Action::set_has_action() {
  _has_bits_[0] |= 0x00000001u;
}
inline void Action::clear_has_action() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void Action::clear_action() {
  action_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  clear_has_action();
}
inline const ::std::string& Action::action() const {
  // @@protoc_insertion_point(field_get:pb.Action.action)
  return action_.GetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void Action::set_action(const ::std::string& value) {
  set_has_action();
  action_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:pb.Action.action)
}
inline void Action::set_action(const char* value) {
  set_has_action();
  action_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:pb.Action.action)
}
inline void Action::set_action(const char* value, size_t size) {
  set_has_action();
  action_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:pb.Action.action)
}
inline ::std::string* Action::mutable_action() {
  set_has_action();
  // @@protoc_insertion_point(field_mutable:pb.Action.action)
  return action_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* Action::release_action() {
  // @@protoc_insertion_point(field_release:pb.Action.action)
  clear_has_action();
  return action_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void Action::set_allocated_action(::std::string* action) {
  if (action != NULL) {
    set_has_action();
  } else {
    clear_has_action();
  }
  action_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), action);
  // @@protoc_insertion_point(field_set_allocated:pb.Action.action)
}

// required .pb.ActionVector pos = 2;
inline bool Action::has_pos() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void Action::set_has_pos() {
  _has_bits_[0] |= 0x00000002u;
}
inline void Action::clear_has_pos() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void Action::clear_pos() {
  if (pos_ != NULL) pos_->::pb::ActionVector::Clear();
  clear_has_pos();
}
inline const ::pb::ActionVector& Action::pos() const {
  // @@protoc_insertion_point(field_get:pb.Action.pos)
  return pos_ != NULL ? *pos_
                         : *::pb::ActionVector::internal_default_instance();
}
inline ::pb::ActionVector* Action::mutable_pos() {
  set_has_pos();
  if (pos_ == NULL) {
    pos_ = new ::pb::ActionVector;
  }
  // @@protoc_insertion_point(field_mutable:pb.Action.pos)
  return pos_;
}
inline ::pb::ActionVector* Action::release_pos() {
  // @@protoc_insertion_point(field_release:pb.Action.pos)
  clear_has_pos();
  ::pb::ActionVector* temp = pos_;
  pos_ = NULL;
  return temp;
}
inline void Action::set_allocated_pos(::pb::ActionVector* pos) {
  delete pos_;
  pos_ = pos;
  if (pos) {
    set_has_pos();
  } else {
    clear_has_pos();
  }
  // @@protoc_insertion_point(field_set_allocated:pb.Action.pos)
}

// required .pb.ActionVector dir = 3;
inline bool Action::has_dir() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void Action::set_has_dir() {
  _has_bits_[0] |= 0x00000004u;
}
inline void Action::clear_has_dir() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void Action::clear_dir() {
  if (dir_ != NULL) dir_->::pb::ActionVector::Clear();
  clear_has_dir();
}
inline const ::pb::ActionVector& Action::dir() const {
  // @@protoc_insertion_point(field_get:pb.Action.dir)
  return dir_ != NULL ? *dir_
                         : *::pb::ActionVector::internal_default_instance();
}
inline ::pb::ActionVector* Action::mutable_dir() {
  set_has_dir();
  if (dir_ == NULL) {
    dir_ = new ::pb::ActionVector;
  }
  // @@protoc_insertion_point(field_mutable:pb.Action.dir)
  return dir_;
}
inline ::pb::ActionVector* Action::release_dir() {
  // @@protoc_insertion_point(field_release:pb.Action.dir)
  clear_has_dir();
  ::pb::ActionVector* temp = dir_;
  dir_ = NULL;
  return temp;
}
inline void Action::set_allocated_dir(::pb::ActionVector* dir) {
  delete dir_;
  dir_ = dir;
  if (dir) {
    set_has_dir();
  } else {
    clear_has_dir();
  }
  // @@protoc_insertion_point(field_set_allocated:pb.Action.dir)
}

// required uint32 xml_id = 4;
inline bool Action::has_xml_id() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void Action::set_has_xml_id() {
  _has_bits_[0] |= 0x00000008u;
}
inline void Action::clear_has_xml_id() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void Action::clear_xml_id() {
  xml_id_ = 0u;
  clear_has_xml_id();
}
inline ::google::protobuf::uint32 Action::xml_id() const {
  // @@protoc_insertion_point(field_get:pb.Action.xml_id)
  return xml_id_;
}
inline void Action::set_xml_id(::google::protobuf::uint32 value) {
  set_has_xml_id();
  xml_id_ = value;
  // @@protoc_insertion_point(field_set:pb.Action.xml_id)
}

// required uint32 job = 5;
inline bool Action::has_job() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void Action::set_has_job() {
  _has_bits_[0] |= 0x00000010u;
}
inline void Action::clear_has_job() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void Action::clear_job() {
  job_ = 0u;
  clear_has_job();
}
inline ::google::protobuf::uint32 Action::job() const {
  // @@protoc_insertion_point(field_get:pb.Action.job)
  return job_;
}
inline void Action::set_job(::google::protobuf::uint32 value) {
  set_has_job();
  job_ = value;
  // @@protoc_insertion_point(field_set:pb.Action.job)
}

inline const Action* Action::internal_default_instance() {
  return &Action_default_instance_.get();
}
#endif  // !PROTOBUF_INLINE_NOT_IN_HEADERS
// -------------------------------------------------------------------

// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace pb

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_actions_2eproto__INCLUDED
