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
  std::unique_ptr<BaseAST> exp;

  void Dump() const override;
  void *toKoopaIR(std::vector<const void *> &stmts) const override;
};

class ExpAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> lor_exp;

  void Dump() const override;
  void *toKoopaIR(std::vector<const void *> &stmts) const override;
};

class LOrExpAST : public BaseAST {
 public:
  std::string op;
  std::unique_ptr<BaseAST> lor_exp;
  std::unique_ptr<BaseAST> land_exp;

  void Dump() const override;
  void *toKoopaIR(std::vector<const void *> &stmts) const override;
  void *toBoolKoopaIR(const std::unique_ptr<BaseAST> &exp, std::vector<const void *> &stmts) const;
};

class LAndExpAST : public BaseAST {
 public:
  std::string op;
  std::unique_ptr<BaseAST> land_exp;
  std::unique_ptr<BaseAST> eq_exp;

  void Dump() const override;
  void *toKoopaIR(std::vector<const void *> &stmts) const override;
  void *toBoolKoopaIR(const std::unique_ptr<BaseAST> &exp, std::vector<const void *> &stmts) const;
};

class EqExpAST : public BaseAST {
 public:
  std::string op;
  std::unique_ptr<BaseAST> eq_exp;
  std::unique_ptr<BaseAST> rel_exp;

  void Dump() const override;
  void *toKoopaIR(std::vector<const void *> &stmts) const override;
};

class RelExpAST : public BaseAST {
 public:
  std::string op;
  std::unique_ptr<BaseAST> rel_exp;
  std::unique_ptr<BaseAST> add_exp;

  void Dump() const override;
  void *toKoopaIR(std::vector<const void *> &stmts) const override;
};

class AddExpAST : public BaseAST {
 public:
  std::string op;
  std::unique_ptr<BaseAST> add_exp;
  std::unique_ptr<BaseAST> mul_exp;

  void Dump() const override;
  void *toKoopaIR(std::vector<const void *> &stmts) const override;
};

class MulExpAST : public BaseAST {
 public:
  std::string op;
  std::unique_ptr<BaseAST> mul_exp;
  std::unique_ptr<BaseAST> unary_exp;

  void Dump() const override;
  void *toKoopaIR(std::vector<const void *> &stmts) const override;
};

class UnaryExpAST : public BaseAST {
 public:
  std::string tag;
  std::string unary_op;
  std::unique_ptr<BaseAST> unary_exp;
  std::unique_ptr<BaseAST> primary_exp;

  void Dump() const override;
  void *toKoopaIR(std::vector<const void *> &stmts) const override;
};

class PrimaryExpAST : public BaseAST {
 public:
  std::string tag;
  std::unique_ptr<BaseAST> exp;
  std::unique_ptr<BaseAST> number;

  void Dump() const override;
  void *toKoopaIR(std::vector<const void *> &stmts) const override;
};

// NumberAST 派生类
class NumberAST : public BaseAST {
 public:
  int int_const;

  NumberAST(){};
  NumberAST(int num);
  void Dump() const override;
  void *toKoopaIR() const override;
};
