#pragma once
#include <stdint.h>
#include <variant>
#include <string>
#include <vector>

namespace parser::ast {

struct Node {
    enum class Kind {
        IntLit, 
        ExprFuncCall, BinExpr, 
        ReturnStmt,
        Block,
        FuncDecl, IntDecl,
        VarRef,
        Program,
    };
    Kind kind;
    explicit Node(Kind k): kind(k) {}
    virtual ~Node() = default;
};

enum class Op {
    ADD,
    SUB,
    MUL,
    DIV
};

enum Types {
    INT,
    VOID
};

struct ExprFuncCall;

struct IntLit : Node {
    std::string val;
    explicit IntLit(std::string v) : Node{Kind::IntLit}, val{std::move(v)} {}
};

struct BinExpr : Node {
    Op op; 
    Node* lhs;
    Node* rhs;
    BinExpr(Op op, Node* l, Node* r) : Node{Kind::BinExpr}, op{op}, lhs{l}, rhs{r} {}
};

struct IntDecl : Node {
    std::string name;
    Node* val;
    IntDecl(std::string name, Node* val) : Node{Kind::IntDecl}, name{std::move(name)}, val{val}  {}
};

struct ExprFuncCall : Node {
    std::string name;
    std::vector<Node*> args;
    ExprFuncCall(std::string name, std::vector<Node*> args) : Node{Kind::ExprFuncCall}, name{std::move(name)}, args{args}  {}
};

struct ReturnStmt : Node {
    Types type;
    Node* val;
    ReturnStmt(Types type, Node* val) : Node{Kind::ReturnStmt}, type{type}, val{val}  {}
};

struct Block : Node {
    std::vector<Node*> content;
    explicit Block(std::vector<Node*> content) : Node{Kind::Block}, content{content}  {}
};

struct FuncDecl : Node{
    std::string name;
    std::vector<Types> args;
    Types return_type;
    Block* content;
    FuncDecl(std::string name, std::vector<Types> args, Types return_type, Block* content) 
        : Node{Kind::FuncDecl}, name{std::move(name)}, args{args}, return_type{return_type}, content{content}  {}
};

struct Program : Node {
    std::vector<Node*> decls;
    explicit Program(std::vector<Node*> decls) : Node{Kind::Program}, decls{decls} {}
};

}
