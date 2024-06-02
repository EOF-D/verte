/**
 * @file types.cpp
 * @brief Tests for the types.
 */

#include "excerpt/types.hpp"
#include <gtest/gtest.h>

using namespace excerpt;

TEST(TypeInfoTest, TestDefaultConstructor) {
  TypeInfo typeInfo;
  EXPECT_EQ(typeInfo.getDataType(), TypeInfo::DataType::UNKNOWN);
  EXPECT_EQ(typeInfo.getName(), "UNKNOWN");
}

TEST(TypeInfoTest, TestConstructor) {
  TypeInfo typeInfo(TypeInfo::DataType::INTEGER, "int");
  EXPECT_EQ(typeInfo.getDataType(), TypeInfo::DataType::INTEGER);
  EXPECT_EQ(typeInfo.getName(), "int");
}

TEST(TypeInfoTest, TestDataTypeConstructor) {
  TypeInfo typeInfo(TypeInfo::DataType::FLOAT);
  EXPECT_EQ(typeInfo.getDataType(), TypeInfo::DataType::FLOAT);
  EXPECT_EQ(typeInfo.getName(), "float");
}

TEST(TypeInfoTest, TestToDataType) {
  EXPECT_EQ(TypeInfo::toDataType("int"), TypeInfo::DataType::INTEGER);
  EXPECT_EQ(TypeInfo::toDataType("float"), TypeInfo::DataType::FLOAT);
  EXPECT_EQ(TypeInfo::toDataType("double"), TypeInfo::DataType::DOUBLE);
  EXPECT_EQ(TypeInfo::toDataType("string"), TypeInfo::DataType::STRING);
  EXPECT_EQ(TypeInfo::toDataType("bool"), TypeInfo::DataType::BOOL);
  EXPECT_EQ(TypeInfo::toDataType("void"), TypeInfo::DataType::VOID);
  EXPECT_EQ(TypeInfo::toDataType("unknown"), TypeInfo::DataType::UNKNOWN);
}

TEST(TypeInfoTest, TestToString) {
  EXPECT_EQ(TypeInfo::toString(TypeInfo::DataType::INTEGER), "int");
  EXPECT_EQ(TypeInfo::toString(TypeInfo::DataType::FLOAT), "float");
  EXPECT_EQ(TypeInfo::toString(TypeInfo::DataType::DOUBLE), "double");
  EXPECT_EQ(TypeInfo::toString(TypeInfo::DataType::STRING), "string");
  EXPECT_EQ(TypeInfo::toString(TypeInfo::DataType::BOOL), "bool");
  EXPECT_EQ(TypeInfo::toString(TypeInfo::DataType::VOID), "void");
  EXPECT_EQ(TypeInfo::toString(TypeInfo::DataType::UNKNOWN), "unknown");
}

TEST(ParameterTest, TestConstructor) {
  Parameter parameter("value", TypeInfo(TypeInfo::DataType::INTEGER, "int"));
  EXPECT_EQ(parameter.name, "value");
  EXPECT_EQ(parameter.typeInfo.getDataType(), TypeInfo::DataType::INTEGER);
  EXPECT_EQ(parameter.typeInfo.getName(), "int");
}
