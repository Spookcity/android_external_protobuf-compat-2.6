// Protocol Buffers - Google's data interchange format
// Copyright 2008 Google Inc.  All rights reserved.
// https://developers.google.com/protocol-buffers/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Author: kenton@google.com (Kenton Varda)

#include <string>
#include <iostream>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/test_util_lite.h>
#include <google/protobuf/unittest_lite.pb.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/wire_format_lite_inl.h>

using namespace std;

namespace {
// Helper methods to test parsing merge behavior.
void ExpectMessageMerged(const gxxgle::protobuf::unittest::TestAllTypesLite& message) {
  GOOGLE_CHECK(message.optional_int32() == 3);
  GOOGLE_CHECK(message.optional_int64() == 2);
  GOOGLE_CHECK(message.optional_string() == "hello");
}

void AssignParsingMergeMessages(
    gxxgle::protobuf::unittest::TestAllTypesLite* msg1,
    gxxgle::protobuf::unittest::TestAllTypesLite* msg2,
    gxxgle::protobuf::unittest::TestAllTypesLite* msg3) {
  msg1->set_optional_int32(1);
  msg2->set_optional_int64(2);
  msg3->set_optional_int32(3);
  msg3->set_optional_string("hello");
}

void SetAllTypesInEmptyMessageUnknownFields(
    gxxgle::protobuf::unittest::TestEmptyMessageLite* empty_message) {
  protobuf_unittest::TestAllTypesLite message;
  gxxgle::protobuf::TestUtilLite::ExpectClear(message);
  gxxgle::protobuf::TestUtilLite::SetAllFields(&message);
  string data = message.SerializeAsString();
  empty_message->ParseFromString(data);
}

void SetSomeTypesInEmptyMessageUnknownFields(
    gxxgle::protobuf::unittest::TestEmptyMessageLite* empty_message) {
  protobuf_unittest::TestAllTypesLite message;
  gxxgle::protobuf::TestUtilLite::ExpectClear(message);
  message.set_optional_int32(101);
  message.set_optional_int64(102);
  message.set_optional_uint32(103);
  message.set_optional_uint64(104);
  string data = message.SerializeAsString();
  empty_message->ParseFromString(data);
}

}  // namespace

int main(int argc, char* argv[]) {
  string data, data2, packed_data;

  {
    protobuf_unittest::TestAllTypesLite message, message2, message3;
    gxxgle::protobuf::TestUtilLite::ExpectClear(message);
    gxxgle::protobuf::TestUtilLite::SetAllFields(&message);
    message2.CopyFrom(message);
    data = message.SerializeAsString();
    message3.ParseFromString(data);
    gxxgle::protobuf::TestUtilLite::ExpectAllFieldsSet(message);
    gxxgle::protobuf::TestUtilLite::ExpectAllFieldsSet(message2);
    gxxgle::protobuf::TestUtilLite::ExpectAllFieldsSet(message3);
    gxxgle::protobuf::TestUtilLite::ModifyRepeatedFields(&message);
    gxxgle::protobuf::TestUtilLite::ExpectRepeatedFieldsModified(message);
    message.Clear();
    gxxgle::protobuf::TestUtilLite::ExpectClear(message);
  }

  {
    protobuf_unittest::TestAllExtensionsLite message, message2, message3;
    gxxgle::protobuf::TestUtilLite::ExpectExtensionsClear(message);
    gxxgle::protobuf::TestUtilLite::SetAllExtensions(&message);
    message2.CopyFrom(message);
    string extensions_data = message.SerializeAsString();
    message3.ParseFromString(extensions_data);
    gxxgle::protobuf::TestUtilLite::ExpectAllExtensionsSet(message);
    gxxgle::protobuf::TestUtilLite::ExpectAllExtensionsSet(message2);
    gxxgle::protobuf::TestUtilLite::ExpectAllExtensionsSet(message3);
    gxxgle::protobuf::TestUtilLite::ModifyRepeatedExtensions(&message);
    gxxgle::protobuf::TestUtilLite::ExpectRepeatedExtensionsModified(message);
    message.Clear();
    gxxgle::protobuf::TestUtilLite::ExpectExtensionsClear(message);
  }

  {
    protobuf_unittest::TestPackedTypesLite message, message2, message3;
    gxxgle::protobuf::TestUtilLite::ExpectPackedClear(message);
    gxxgle::protobuf::TestUtilLite::SetPackedFields(&message);
    message2.CopyFrom(message);
    packed_data = message.SerializeAsString();
    message3.ParseFromString(packed_data);
    gxxgle::protobuf::TestUtilLite::ExpectPackedFieldsSet(message);
    gxxgle::protobuf::TestUtilLite::ExpectPackedFieldsSet(message2);
    gxxgle::protobuf::TestUtilLite::ExpectPackedFieldsSet(message3);
    gxxgle::protobuf::TestUtilLite::ModifyPackedFields(&message);
    gxxgle::protobuf::TestUtilLite::ExpectPackedFieldsModified(message);
    message.Clear();
    gxxgle::protobuf::TestUtilLite::ExpectPackedClear(message);
  }

  {
    protobuf_unittest::TestPackedExtensionsLite message, message2, message3;
    gxxgle::protobuf::TestUtilLite::ExpectPackedExtensionsClear(message);
    gxxgle::protobuf::TestUtilLite::SetPackedExtensions(&message);
    message2.CopyFrom(message);
    string packed_extensions_data = message.SerializeAsString();
    GOOGLE_CHECK(packed_extensions_data == packed_data);
    message3.ParseFromString(packed_extensions_data);
    gxxgle::protobuf::TestUtilLite::ExpectPackedExtensionsSet(message);
    gxxgle::protobuf::TestUtilLite::ExpectPackedExtensionsSet(message2);
    gxxgle::protobuf::TestUtilLite::ExpectPackedExtensionsSet(message3);
    gxxgle::protobuf::TestUtilLite::ModifyPackedExtensions(&message);
    gxxgle::protobuf::TestUtilLite::ExpectPackedExtensionsModified(message);
    message.Clear();
    gxxgle::protobuf::TestUtilLite::ExpectPackedExtensionsClear(message);
  }

  {
    // Test that if an optional or required message/group field appears multiple
    // times in the input, they need to be merged.
    gxxgle::protobuf::unittest::TestParsingMergeLite::RepeatedFieldsGenerator generator;
    gxxgle::protobuf::unittest::TestAllTypesLite* msg1;
    gxxgle::protobuf::unittest::TestAllTypesLite* msg2;
    gxxgle::protobuf::unittest::TestAllTypesLite* msg3;

#define ASSIGN_REPEATED_FIELD(FIELD)                \
  msg1 = generator.add_##FIELD();                   \
  msg2 = generator.add_##FIELD();                   \
  msg3 = generator.add_##FIELD();                   \
  AssignParsingMergeMessages(msg1, msg2, msg3)

    ASSIGN_REPEATED_FIELD(field1);
    ASSIGN_REPEATED_FIELD(field2);
    ASSIGN_REPEATED_FIELD(field3);
    ASSIGN_REPEATED_FIELD(ext1);
    ASSIGN_REPEATED_FIELD(ext2);

#undef ASSIGN_REPEATED_FIELD
#define ASSIGN_REPEATED_GROUP(FIELD)                \
  msg1 = generator.add_##FIELD()->mutable_field1(); \
  msg2 = generator.add_##FIELD()->mutable_field1(); \
  msg3 = generator.add_##FIELD()->mutable_field1(); \
  AssignParsingMergeMessages(msg1, msg2, msg3)

    ASSIGN_REPEATED_GROUP(group1);
    ASSIGN_REPEATED_GROUP(group2);

#undef ASSIGN_REPEATED_GROUP

    string buffer;
    generator.SerializeToString(&buffer);
    gxxgle::protobuf::unittest::TestParsingMergeLite parsing_merge;
    parsing_merge.ParseFromString(buffer);

    // Required and optional fields should be merged.
    ExpectMessageMerged(parsing_merge.required_all_types());
    ExpectMessageMerged(parsing_merge.optional_all_types());
    ExpectMessageMerged(
        parsing_merge.optionalgroup().optional_group_all_types());
    ExpectMessageMerged(parsing_merge.GetExtension(
        gxxgle::protobuf::unittest::TestParsingMergeLite::optional_ext));

    // Repeated fields should not be merged.
    GOOGLE_CHECK(parsing_merge.repeated_all_types_size() == 3);
    GOOGLE_CHECK(parsing_merge.repeatedgroup_size() == 3);
    GOOGLE_CHECK(parsing_merge.ExtensionSize(
        gxxgle::protobuf::unittest::TestParsingMergeLite::repeated_ext) == 3);
  }

  // Test unknown fields support for lite messages.
  {
    protobuf_unittest::TestAllTypesLite message, message2;
    protobuf_unittest::TestEmptyMessageLite empty_message;
    gxxgle::protobuf::TestUtilLite::ExpectClear(message);
    gxxgle::protobuf::TestUtilLite::SetAllFields(&message);
    data = message.SerializeAsString();
    empty_message.ParseFromString(data);
    data.clear();
    data = empty_message.SerializeAsString();
    message2.ParseFromString(data);
    data = message2.SerializeAsString();
    gxxgle::protobuf::TestUtilLite::ExpectAllFieldsSet(message2);
    message.Clear();
    gxxgle::protobuf::TestUtilLite::ExpectClear(message);
  }

  {
    protobuf_unittest::TestAllExtensionsLite message, message2;
    protobuf_unittest::TestEmptyMessageLite empty_message;
    gxxgle::protobuf::TestUtilLite::ExpectExtensionsClear(message);
    gxxgle::protobuf::TestUtilLite::SetAllExtensions(&message);
    data = message.SerializeAsString();
    empty_message.ParseFromString(data);
    data.clear();
    data = empty_message.SerializeAsString();
    message2.ParseFromString(data);
    data = message2.SerializeAsString();
    gxxgle::protobuf::TestUtilLite::ExpectAllExtensionsSet(message2);
    message.Clear();
    gxxgle::protobuf::TestUtilLite::ExpectExtensionsClear(message);
  }

  {
    protobuf_unittest::TestPackedTypesLite message, message2;
    protobuf_unittest::TestEmptyMessageLite empty_message;
    gxxgle::protobuf::TestUtilLite::ExpectPackedClear(message);
    gxxgle::protobuf::TestUtilLite::SetPackedFields(&message);
    data = message.SerializeAsString();
    empty_message.ParseFromString(data);
    data.clear();
    data = empty_message.SerializeAsString();
    message2.ParseFromString(data);
    data = message2.SerializeAsString();
    gxxgle::protobuf::TestUtilLite::ExpectPackedFieldsSet(message2);
    message.Clear();
    gxxgle::protobuf::TestUtilLite::ExpectPackedClear(message);
  }

  {
    protobuf_unittest::TestPackedExtensionsLite message, message2;
    protobuf_unittest::TestEmptyMessageLite empty_message;
    gxxgle::protobuf::TestUtilLite::ExpectPackedExtensionsClear(message);
    gxxgle::protobuf::TestUtilLite::SetPackedExtensions(&message);
    data = message.SerializeAsString();
    empty_message.ParseFromString(data);
    data.clear();
    data = empty_message.SerializeAsString();
    message2.ParseFromString(data);
    data = message2.SerializeAsString();
    gxxgle::protobuf::TestUtilLite::ExpectPackedExtensionsSet(message2);
    message.Clear();
    gxxgle::protobuf::TestUtilLite::ExpectPackedExtensionsClear(message);
  }

  {
    // Test Unknown fields swap
    protobuf_unittest::TestEmptyMessageLite empty_message, empty_message2;
    SetAllTypesInEmptyMessageUnknownFields(&empty_message);
    SetSomeTypesInEmptyMessageUnknownFields(&empty_message2);
    data = empty_message.SerializeAsString();
    data2 = empty_message2.SerializeAsString();
    empty_message.Swap(&empty_message2);
    GOOGLE_CHECK_EQ(data, empty_message2.SerializeAsString());
    GOOGLE_CHECK_EQ(data2, empty_message.SerializeAsString());
  }

  {
    // Test unknown fields swap with self
    protobuf_unittest::TestEmptyMessageLite empty_message;
    SetAllTypesInEmptyMessageUnknownFields(&empty_message);
    data = empty_message.SerializeAsString();
    empty_message.Swap(&empty_message);
    GOOGLE_CHECK_EQ(data, empty_message.SerializeAsString());
  }

  {
    // Test MergeFrom with unknown fields
    protobuf_unittest::TestAllTypesLite message, message2;
    protobuf_unittest::TestEmptyMessageLite empty_message, empty_message2;
    message.set_optional_int32(101);
    message.add_repeated_int32(201);
    message.set_optional_nested_enum(gxxgle::protobuf::unittest::TestAllTypesLite::BAZ);
    message2.set_optional_int64(102);
    message2.add_repeated_int64(202);
    message2.set_optional_foreign_enum(gxxgle::protobuf::unittest::FOREIGN_LITE_BAZ);

    data = message.SerializeAsString();
    empty_message.ParseFromString(data);
    data = message2.SerializeAsString();
    empty_message2.ParseFromString(data);
    message.MergeFrom(message2);
    empty_message.MergeFrom(empty_message2);

    data = empty_message.SerializeAsString();
    message2.ParseFromString(data);
    // We do not compare the serialized output of a normal message and a lite
    // message because the order of fields do not match. We convert lite message
    // back into normal message, then compare.
    GOOGLE_CHECK_EQ(message.SerializeAsString(), message2.SerializeAsString());
  }

  {
    // Test unknown enum value
    protobuf_unittest::TestAllTypesLite message;
    string buffer;
    {
      gxxgle::protobuf::io::StringOutputStream output_stream(&buffer);
      gxxgle::protobuf::io::CodedOutputStream coded_output(&output_stream);
      gxxgle::protobuf::internal::WireFormatLite::WriteTag(
          protobuf_unittest::TestAllTypesLite::kOptionalNestedEnumFieldNumber,
          gxxgle::protobuf::internal::WireFormatLite::WIRETYPE_VARINT, &coded_output);
      coded_output.WriteVarint32(10);
      gxxgle::protobuf::internal::WireFormatLite::WriteTag(
          protobuf_unittest::TestAllTypesLite::kRepeatedNestedEnumFieldNumber,
          gxxgle::protobuf::internal::WireFormatLite::WIRETYPE_VARINT, &coded_output);
      coded_output.WriteVarint32(20);
    }
    message.ParseFromString(buffer);
    data = message.SerializeAsString();
    GOOGLE_CHECK_EQ(data, buffer);
  }

  {
    // Test Clear with unknown fields
    protobuf_unittest::TestEmptyMessageLite empty_message;
    SetAllTypesInEmptyMessageUnknownFields(&empty_message);
    empty_message.Clear();
    GOOGLE_CHECK_EQ(0, empty_message.unknown_fields().size());
  }

  cout << "PASS" << endl;
  return 0;
}
