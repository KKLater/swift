//===--- Parser.h - Swift Language Parser -----------------------*- C++ -*-===//
//
// This source file is part of the Swift.org open source project
//
// Copyright (c) 2014 - 2015 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See http://swift.org/LICENSE.txt for license information
// See http://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//
//===----------------------------------------------------------------------===//
//
//  This file defines the Parser interface.
//
//===----------------------------------------------------------------------===//

#ifndef SWIFT_PARSER_H
#define SWIFT_PARSER_H

#include "swift/Parse/Token.h"

namespace llvm {
  class SourceMgr;
  template <typename PT1, typename PT2>
  class PointerUnion;
  template<class T>
  class NullablePtr;
  class Twine;
}

namespace swift {
  class Lexer;
  class Sema;
  class Expr;
  class Type;
  class Decl;
  class DeclAttributes;
  class TranslationUnitDecl;
  class TypeAliasDecl;
  class FuncDecl;
  class VarDecl;
  class ASTContext;
  class NameRecord;
  class TupleTypeElt;
  class Identifier;
  
class Parser {
  llvm::SourceMgr &SourceMgr;
  Lexer &L;
  Sema &S;
  
  /// Tok - This is the current token being considered by the parser.
  Token Tok;
  
  Parser(const Parser&);         // DO NOT IMPLEMENT
  void operator=(const Parser&); // DO NOT IMPLEMENT
public:
  Parser(unsigned BufferID, ASTContext &Ctx);
  ~Parser();
  
  TranslationUnitDecl *parseTranslationUnit();
  
private:
  // Utilities.
  void consumeToken();
  void consumeToken(tok::TokenKind K) {
    assert(Tok.is(K) && "Consuming wrong token kind");
    consumeToken();
  }
  
  /// consumeIf - If the current token is the specified kind, consume it and
  /// return true.  Otherwise, return false without consuming it.
  bool consumeIf(tok::TokenKind K) {
    if (Tok.isNot(K)) return false;
    consumeToken(K);
    return true;
  }
  
  /// skipUntil - Read tokens until we get to the specified token, then return
  /// without consuming it.  Because we cannot guarantee that the token will
  /// ever occur, this skips to some likely good stopping point.
  ///
  void skipUntil(tok::TokenKind T);
  
  void note(llvm::SMLoc Loc, const llvm::Twine &Message);
  void warning(llvm::SMLoc Loc, const llvm::Twine &Message);
  void error(llvm::SMLoc Loc, const llvm::Twine &Message);
  
  // Primitive Parsing
  bool parseIdentifier(Identifier &Result, const llvm::Twine &Message);

  /// parseToken - The parser expects that 'K' is next in the input.  If so, it
  /// is consumed and false is returned.
  ///
  /// If the input is malformed, this emits the specified error diagnostic.
  /// Next, if SkipToTok is specified, it calls skipUntil(SkipToTok).  Finally,
  /// true is returned.
  bool parseToken(tok::TokenKind K, const char *Message,
                  tok::TokenKind SkipToTok = tok::unknown);
  
  bool parseValueSpecifier(Type *&Ty, llvm::NullablePtr<Expr> &Init);
  
  // Decl Parsing
  Decl *parseDeclTopLevel();
  TypeAliasDecl *parseDeclTypeAlias();
  void parseAttributeList(DeclAttributes &Attributes);
  bool parseAttribute(DeclAttributes &Attributes);
  bool parseVarName(NameRecord &Record);
  
  Decl *parseDeclOneOf();
  Decl *parseDeclStruct();
  VarDecl *parseDeclVar();
  FuncDecl *parseDeclFunc();
  
  // Type Parsing
  bool parseType(Type *&Result);
  bool parseType(Type *&Result, const llvm::Twine &Message);
  bool parseTypeTuple(Type *&Result);
  bool parseTypeOneOfBody(llvm::SMLoc OneOfLoc, const DeclAttributes &Attrs,
                          Type *&Result);

  // Expression Parsing
  bool isStartOfExpr(Token &Tok) const;
  bool parseExpr(llvm::NullablePtr<Expr> &Result, const char *Message = 0);
  bool parseExprSingle(llvm::NullablePtr<Expr> &Result, const char *Message =0);
  bool parseExprPrimary(llvm::NullablePtr<Expr> &Result, const char *Message=0);
  bool parseExprIdentifier(llvm::NullablePtr<Expr> &Result);
  bool parseExprParen(llvm::NullablePtr<Expr> &Result);
  bool parseExprBrace(llvm::NullablePtr<Expr> &Result);
  bool parseExprBinaryRHS(llvm::NullablePtr<Expr> &Result,
                          unsigned MinPrecedence = 0);
};
  
} // end namespace swift

#endif
