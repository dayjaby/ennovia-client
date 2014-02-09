// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: msg.proto

#ifndef PROTOBUF_msg_2eproto__INCLUDED
#define PROTOBUF_msg_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2005000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2005000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace Ennovia {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_msg_2eproto();
void protobuf_AssignDesc_msg_2eproto();
void protobuf_ShutdownFile_msg_2eproto();

class MessageHeader;

// ===================================================================

class MessageHeader : public ::google::protobuf::Message {
 public:
  MessageHeader();
  virtual ~MessageHeader();

  MessageHeader(const MessageHeader& from);

  inline MessageHeader& operator=(const MessageHeader& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const MessageHeader& default_instance();

  void Swap(MessageHeader* other);

  // implements Message ----------------------------------------------

  MessageHeader* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const MessageHeader& from);
  void MergeFrom(const MessageHeader& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required int32 id = 1;
  inline bool has_id() const;
  inline void clear_id();
  static const int kIdFieldNumber = 1;
  inline ::google::protobuf::int32 id() const;
  inline void set_id(::google::protobuf::int32 value);

  // required int32 size = 2;
  inline bool has_size() const;
  inline void clear_size();
  static const int kSizeFieldNumber = 2;
  inline ::google::protobuf::int32 size() const;
  inline void set_size(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:Ennovia.MessageHeader)
 private:
  inline void set_has_id();
  inline void clear_has_id();
  inline void set_has_size();
  inline void clear_has_size();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::int32 id_;
  ::google::protobuf::int32 size_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];

  friend void  protobuf_AddDesc_msg_2eproto();
  friend void protobuf_AssignDesc_msg_2eproto();
  friend void protobuf_ShutdownFile_msg_2eproto();

  void InitAsDefaultInstance();
  static MessageHeader* default_instance_;
};
// ===================================================================


// ===================================================================

// MessageHeader

// required int32 id = 1;
inline bool MessageHeader::has_id() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void MessageHeader::set_has_id() {
  _has_bits_[0] |= 0x00000001u;
}
inline void MessageHeader::clear_has_id() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void MessageHeader::clear_id() {
  id_ = 0;
  clear_has_id();
}
inline ::google::protobuf::int32 MessageHeader::id() const {
  return id_;
}
inline void MessageHeader::set_id(::google::protobuf::int32 value) {
  set_has_id();
  id_ = value;
}

// required int32 size = 2;
inline bool MessageHeader::has_size() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void MessageHeader::set_has_size() {
  _has_bits_[0] |= 0x00000002u;
}
inline void MessageHeader::clear_has_size() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void MessageHeader::clear_size() {
  size_ = 0;
  clear_has_size();
}
inline ::google::protobuf::int32 MessageHeader::size() const {
  return size_;
}
inline void MessageHeader::set_size(::google::protobuf::int32 value) {
  set_has_size();
  size_ = value;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace Ennovia

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_msg_2eproto__INCLUDED
