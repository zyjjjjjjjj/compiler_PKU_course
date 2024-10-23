#pragma once
#include <memory>
#include <string>
#include <iostream>

class BaseIR {
 public:
  virtual ~BaseIR() = default;
  virtual void Dump() const = 0;
};

class Program : public BaseIR {
 public:
  std::unique_ptr<BaseIR> function;

  void Dump() const override {
    function->Dump();
  }
};

class Function : public BaseIR {
 public:
  std::string fun_type;
  std::string fun_name;
  std::unique_ptr<BaseIR> basic_block;

  void Dump() const override {
    std::cout << "fun @" << fun_name << "(): " << fun_type << " {\n";
    basic_block->Dump();
    std::cout << "}\n";
  }
};

class BasicBlock : public BaseIR {
 public:
  std::unique_ptr<BaseIR> value;

  void Dump() const override {
    std::cout<<"%entry:\n";
    value->Dump();
  }
};

class Value : public BaseIR {
 public:
  std::string instruction;
  int number;

  void Dump() const override {
    std::cout << "\t" << instruction << " " << number << "\n";
  }
};