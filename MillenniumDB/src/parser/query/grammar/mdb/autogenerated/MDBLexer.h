
// Generated from MDBLexer.g4 by ANTLR 4.9.3

#pragma once


#include "antlr4-runtime.h"




class  MDBLexer : public antlr4::Lexer {
public:
  enum {
    K_ACYCLIC = 1, K_ANY_ACYCLIC = 2, K_ANY_SIMPLE = 3, K_ANY_TRAILS = 4, 
    K_ANY = 5, K_AND = 6, K_AVG = 7, K_ALL_COUNT = 8, K_ALL_SHORTEST_ACYCLIC = 9, 
    K_ALL_SHORTEST_SIMPLE = 10, K_ALL_SHORTEST_TRAILS = 11, K_ALL = 12, 
    K_ASC = 13, K_BY = 14, K_BOOL = 15, K_COUNT = 16, K_DESCRIBE = 17, K_DESC = 18, 
    K_DISTINCT = 19, K_EDGE = 20, K_INTEGER = 21, K_INSERT = 22, K_IS = 23, 
    K_FALSE = 24, K_FLOAT = 25, K_GROUP = 26, K_LABEL = 27, K_LIMIT = 28, 
    K_MAX = 29, K_MATCH = 30, K_MIN = 31, K_OPTIONAL = 32, K_ORDER = 33, 
    K_OR = 34, K_PROPERTY = 35, K_NOT = 36, K_NULL = 37, K_RETURN = 38, 
    K_SET = 39, K_SIMPLE = 40, K_STRING = 41, K_SUM = 42, K_TRAILS = 43, 
    K_TRUE = 44, K_WHERE = 45, TRUE_PROP = 46, FALSE_PROP = 47, ANON_ID = 48, 
    EDGE_ID = 49, KEY = 50, TYPE = 51, TYPE_VAR = 52, VARIABLE = 53, STRING = 54, 
    UNSIGNED_INTEGER = 55, UNSIGNED_FLOAT = 56, NAME = 57, LEQ = 58, GEQ = 59, 
    EQ = 60, NEQ = 61, LT = 62, GT = 63, SINGLE_EQ = 64, PATH_SEQUENCE = 65, 
    PATH_ALTERNATIVE = 66, PATH_NEGATION = 67, STAR = 68, PERCENT = 69, 
    QUESTION_MARK = 70, PLUS = 71, MINUS = 72, L_PAR = 73, R_PAR = 74, LCURLY_BRACKET = 75, 
    RCURLY_BRACKET = 76, LSQUARE_BRACKET = 77, RSQUARE_BRACKET = 78, COMMA = 79, 
    COLON = 80, WHITE_SPACE = 81, SINGLE_LINE_COMMENT = 82, UNRECOGNIZED = 83
  };

  enum {
    WS_CHANNEL = 2
  };

  explicit MDBLexer(antlr4::CharStream *input);
  ~MDBLexer();

  virtual std::string getGrammarFileName() const override;
  virtual const std::vector<std::string>& getRuleNames() const override;

  virtual const std::vector<std::string>& getChannelNames() const override;
  virtual const std::vector<std::string>& getModeNames() const override;
  virtual const std::vector<std::string>& getTokenNames() const override; // deprecated, use vocabulary instead
  virtual antlr4::dfa::Vocabulary& getVocabulary() const override;

  virtual const std::vector<uint16_t> getSerializedATN() const override;
  virtual const antlr4::atn::ATN& getATN() const override;

private:
  static std::vector<antlr4::dfa::DFA> _decisionToDFA;
  static antlr4::atn::PredictionContextCache _sharedContextCache;
  static std::vector<std::string> _ruleNames;
  static std::vector<std::string> _tokenNames;
  static std::vector<std::string> _channelNames;
  static std::vector<std::string> _modeNames;

  static std::vector<std::string> _literalNames;
  static std::vector<std::string> _symbolicNames;
  static antlr4::dfa::Vocabulary _vocabulary;
  static antlr4::atn::ATN _atn;
  static std::vector<uint16_t> _serializedATN;


  // Individual action functions triggered by action() above.

  // Individual semantic predicate functions triggered by sempred() above.

  struct Initializer {
    Initializer();
  };
  static Initializer _init;
};

