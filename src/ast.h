// AST.h
#pragma once

#include <memory>
#include <string>
#include <iostream>
#include "koopaIR.h"

// 基类声明
class BaseAST {
 public:
  virtual ~BaseAST() = default;
  
  virtual void Dump() const = 0;
  virtual BaseIR *toKoopaIR() const { return nullptr; };
};

// CompUnitAST 派生类
class CompUnitAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> func_def;

  void Dump() const override;
  BaseIR *toKoopaIR() const override;
};

// FuncDefAST 派生类
class FuncDefAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> func_type;
  std::string ident;
  std::unique_ptr<BaseAST> block;

  void Dump() const override;
  BaseIR *toKoopaIR() const override;
};

// FuncTypeAST 派生类
class FuncTypeAST : public BaseAST {
 public:
  std::string Int;

  void Dump() const override;
};

// BlockAST 派生类
class BlockAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> stmt;

  void Dump() const override;
  BaseIR *toKoopaIR() const override;
};

// StmtAST 派生类
class StmtAST : public BaseAST {
 public:
  std::string Return;
  int number;

  void Dump() const override;
  BaseIR *toKoopaIR() const override;
};

// NumberAST 派生类
class NumberAST : public BaseAST {
 public:
  int int_const;

  void Dump() const override;
};
