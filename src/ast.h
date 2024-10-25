// AST.h
#pragma once
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include "koopa.h"
#include "utils.h"

// 基类声明
class BaseAST {
 public:
  virtual ~BaseAST() = default;
  
  virtual void Dump() const = 0;
  virtual void *toKoopaIR() const { return nullptr; };
  virtual void *toKoopaIR(std::vector<const void *> &stmts) const { return nullptr; };
};

// CompUnitAST 派生类
class CompUnitAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> func_def;

  void Dump() const override;
  void *toKoopaIR() const override;
};

// FuncDefAST 派生类
class FuncDefAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> func_type;
  std::string ident;
  std::unique_ptr<BaseAST> block;

  void Dump() const override;
  void *toKoopaIR() const override;
};

// FuncTypeAST 派生类
class FuncTypeAST : public BaseAST {
 public:
  std::string func_type;

  void Dump() const override;
  void *toKoopaIR() const override;
};

// BlockAST 派生类
class BlockAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> stmt;

  void Dump() const override;
  void *toKoopaIR() const override;
};

// StmtAST 派生类
class StmtAST : public BaseAST {
 public:
  std::string Return;
  int number;

  void Dump() const override;
  void *toKoopaIR(std::vector<const void *> &stmts) const override;
};

// NumberAST 派生类
class NumberAST : public BaseAST {
 public:
  int int_const;

  void Dump() const override;
};
