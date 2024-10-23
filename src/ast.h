# pragma once
#include <memory>
#include <string>
#include <iostream>

class BaseAST {
 public:
  virtual ~BaseAST() = default;
  
  virtual void Dump() const = 0;
};

// CompUnit 是 BaseAST
class CompUnitAST : public BaseAST {
 public:
  // 用智能指针管理对象
  std::unique_ptr<BaseAST> func_def;

  void Dump() const override {
    std::cout << "CompUnitAST { ";
    func_def->Dump();
    std::cout << " }";
  }
};

class FuncDefAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> func_type;
  std::string ident;
  std::unique_ptr<BaseAST> block;

  void Dump() const override {
    std::cout << "FuncDefAST { ";
    func_type->Dump();
    std::cout << ", " << ident << ", ";
    block->Dump();
    std::cout << " }";
  }
};

class FuncTypeAST : public BaseAST {
 public:
  std::string Int;

  void Dump() const override {
    std::cout << "FuncTypeAST { " << Int << " }";
  }
};

class BlockAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> stmt;

    void Dump() const override {
        std::cout << "BlockAST { ";
        stmt->Dump();
        std::cout << " }";
    }
};

class StmtAST : public BaseAST {
 public:
  std::string Return;
  int number;

    void Dump() const override {
        std::cout << "StmtAST { " << Return << ", " << number << " }";
    }
};

class NumberAST : public BaseAST {
 public:
  int int_const;

    void Dump() const override {
        std::cout << "NumberAST { " << int_const << " }";
    }
};