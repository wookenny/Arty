#pragma once
#include <string>
#include <map>
#include <unordered_map>
#include <utility>
#include <memory>
#include <iostream>
#include <typeinfo>
#include <map>
#include <utility>
#include <cmath>
#include <tuple>
#include <algorithm>
#include "boost/lexical_cast.hpp" 

//all valid operatons, ADD = +/-
enum Operator{ 
	ADD,
	SUB,
	DIV, 
	MULT,
	MOD,
	POW,
	NUM,
	VAR
};

//node for the internal tree for the expression
template <typename T>
struct node{
	typedef T type;	
	
	std::unique_ptr<node<T> > left;
	std::unique_ptr<node<T> > right;
	Operator op; 
	T value;
	
	node(){
		left  = nullptr;
		right = nullptr;
		op = Operator::NUM;
		value = T{};
	}
	
};


//whole class
template <typename T>
class MathFunction{

private:
	
	static std::multimap<int, std::string> _operatorOrder;
	static std::unordered_map<std::string, int> _operatorPriority;
	static std::unordered_map<std::string,Operator> _strToOp;
	static std::map<Operator,std::string> _opToStr;

	std::string _term;
	std::unique_ptr<node<T> > _expression_root;
		
	void _buildExpressionTree(node<T> *n,std::string);
	void _compressTree(node<T>*);
	bool _varInSubTree(node<T>*) const;
	T _evaluateExpressionTree(node<T> *n,T) const;
	
	std::tuple<std::string, Operator, std::string>  _splitTerm(const std::string &) const;
	std::pair<Operator,size_t> _smallestOperator(const std::string&) const;

public:
	typedef T type;	


	void test();
	MathFunction(std::string);
	T evaluate(T) const;
		
	T operator()(T v) const { return _evaluateExpressionTree(_expression_root.get(),v);}
};






//init of the static variable
template<typename T>
std::multimap<int, std::string> MathFunction<T>::_operatorOrder{{1,"+"},{1,"-"},{2,"*"},{2,"/"},{"%",2},{3,"^"}}; 

template<typename T>
std::unordered_map<std::string, int> MathFunction<T>::_operatorPriority{{"+",1},{"-",1},{"*",2},{"/",2},{"%",2},{"^",3},{"x",4}}; 

template<typename T>
std::unordered_map<std::string,Operator> MathFunction<T>::_strToOp{
									{"+",Operator::ADD},
									{"-",Operator::SUB},
									{"*",Operator::MULT},
									{"/",Operator::DIV},
									{"%",Operator::MOD},
									{"^",Operator::POW},
									{"x",Operator::VAR}};
template<typename T> 
std::map<Operator,std::string> MathFunction<T>::_opToStr{
									{Operator::ADD,"+"},
									{Operator::SUB,"-"},
									{Operator::MULT,"*"},
									{Operator::MOD,"%"},
									{Operator::DIV,"/"},
									{Operator::POW,"^"},
									{Operator::VAR,"x"}};


template <typename T> 
std::tuple<std::string, Operator, std::string>  MathFunction<T>::_splitTerm(const std::string &term) const{
	
	//search from tail to head for the operator with the smallest priority, which is not inside a braket 
	int pos = term.size()-1;
	int priority = 100;
	Operator result = Operator::NUM;
	bool operator_found = false;
	int splitpos = -1;	
	int bracket_level = 0;
	while(pos >= 0){
		if(term[pos]==')'){
			++bracket_level;
		}else if(term[pos]=='('){
			--bracket_level;
		}

		// inside a braket?
		if(bracket_level>0){		
			--pos;
			continue;
		}
		
		std::string t = term.substr(pos,1);
		auto op = _strToOp.find(t);
		if(op != _strToOp.end() ){
			auto found_op = op->second;
			if( _operatorPriority[t] < priority ){
				 priority = _operatorPriority[t];
				 result = found_op;
				 operator_found = true;
				 splitpos = pos;
			}
		}
		--pos;
	}			
	
	//end of string and nothing found? => remove outer brakets
	if(not operator_found){
		if(term[0]=='(' and term[term.size()-1]==')'){
				return _splitTerm(term.substr(1,term.size()-2));
		}else{
			if ("x"!= term ){
				return std::make_tuple(term,Operator::NUM,term);
			}else{
				return std::make_tuple(term,Operator::VAR,term);
			}
		}
	}else{
		return std::make_tuple(term.substr(0,splitpos),result,term.substr(splitpos+1,term.size()-splitpos));
	}
}

 
 
template <typename T>
std::pair<Operator,size_t> MathFunction<T>::_smallestOperator(const std::string &term) const{
	size_t pos = std::string::npos;
	int priority = 0;
	Operator result = Operator::NUM;
	
	//run through string, only consider operators with bracket depth == 0
	//always bigger than 0 => remove a wrapping pair of brackets

	for(auto op: _operatorOrder){
		//break: found operator with lower priority
		if(priority!=0 and op.first > priority)
			break;

		size_t new_pos = term.rfind(op.second);
		if(new_pos!=std::string::npos){
			if(pos==std::string::npos){
				pos = new_pos;
				result = _strToOp[op.second];
				priority = op.first;				
			}else{
				if(new_pos>pos){
					pos = new_pos;
					result = _strToOp[op.second];
				}			
			}
		}
	}
	
	//nothing => num
	if(result==Operator::NUM and term.find(_opToStr[Operator::VAR])!=std::string::npos)
		 result = Operator::VAR;
	return std::make_pair(result,pos);	
}



template <typename T>
bool MathFunction<T>::_varInSubTree(node<T> *n) const{
	if (n->op==Operator::NUM)
		return false;
	if (n->op==Operator::VAR)
		return true;
	return _varInSubTree(n->left.get()) or _varInSubTree(n->right.get());
}

template <typename T>
void MathFunction<T>::_compressTree(node<T> *n){

	if(n==nullptr or (n->left==nullptr and n->right==nullptr))
		return;

	if(  not _varInSubTree(n->left.get()) and not _varInSubTree(n->right.get()) ){
		n->value = _evaluateExpressionTree(n,T{}/*does not matter*/);
		//std::cout<<"compessing node with op "<<_opToStr[n->op]<<" to "<<n->value<<std::endl;
		n->left = nullptr;
		n->right = nullptr;
		n->op = Operator::NUM;
		return;
	}
	
	_compressTree(n->left.get());
	_compressTree(n->right.get());				

}


template <typename T>
void MathFunction<T>::_buildExpressionTree(node<T> *n,std::string term){
	///*
	//std::cout<<"examing: "<<term<<std::endl;
	auto splitted = _splitTerm(term);
	
	//std::cout<<"found "<<((_opToStr[std::get<1>(splitted)]=="")?"number":_opToStr[std::get<1>(splitted)])<<std::endl;
	//std::cout<<"left: "<< std::get<0>(splitted)<<std::endl;
	//std::cout<<"right: "<< std::get<2>(splitted)<<"\n\n"<<std::endl;

	n->op = std::get<1>(splitted);
	auto &op =  n->op;
	if(op==Operator::NUM){
		n->value = boost::lexical_cast<T>(std::get<0>(splitted));
	}else if(op==Operator::VAR){
		return;	
	}else{

		
		n->left  = std::move(std::unique_ptr<node<T> >(new node<T>()));
		_buildExpressionTree(n->left.get(), std::get<0>(splitted) );
		n->right = std::move(std::unique_ptr<node<T> >(new node<T>()));
		_buildExpressionTree(n->right.get(), std::get<2>(splitted) );
	}//*/
}

template <typename T>
T MathFunction<T>::_evaluateExpressionTree(node<T> *n,T t) const{
	
	switch(n->op){
		case Operator::NUM  : 
			return n->value;
		case Operator::VAR :
			return t;
		case Operator::ADD  : 
			return    _evaluateExpressionTree(n->left.get(),t)
				+ _evaluateExpressionTree(n->right.get(),t);
		case Operator::SUB  :
			return    _evaluateExpressionTree(n->left.get(),t)
				- _evaluateExpressionTree(n->right.get(),t);
		case Operator::DIV  :
			return    _evaluateExpressionTree(n->left.get(),t)
				/ _evaluateExpressionTree(n->right.get(),t);
		case Operator::MULT :
			return    _evaluateExpressionTree(n->left.get(),t)
				* _evaluateExpressionTree(n->right.get(),t);
		case Operator::MOD :
			return    fmod(_evaluateExpressionTree(n->left.get(),t),
				 _evaluateExpressionTree(n->right.get(),t));
		case Operator::POW :
			return    pow(_evaluateExpressionTree(n->left.get(),t),
				      _evaluateExpressionTree(n->right.get(),t));
		default: return n->value;
	}
}


template <typename T>
MathFunction<T>::MathFunction(std::string s):_term(s){
	
	_expression_root = std::move(std::unique_ptr<node<T> >(new node<T>()));

	std::string copy = _term;
	copy.erase(std::remove_if(copy.begin(), copy.end(), isspace), copy.end());
	_buildExpressionTree(_expression_root.get(), copy);
	_compressTree(_expression_root.get());
}

template <typename T>
T MathFunction<T>::evaluate(T t) const{
	//std::cout<< "Evaluating "<<_term<<" with type "<<typeid(type).name()
	//	<<"\nwith given parameter "<<t<<std::endl;


	T res =  _evaluateExpressionTree(_expression_root.get(),t);
	//std::cout <<_term<<" with x="<<t<<" = "<<res<< std::endl;
	return res;
}


template <typename T>
void MathFunction<T>::test(){
	std::string s = "(((+44)))";
	std::tuple<std::string, Operator, std::string> result = _splitTerm(s);
	std::cout<<"splitting: "<<s<<std::endl;
		
	std::cout<< "result: '"
				<< std::get<0>(result) 
				<< "' "
				<< _opToStr[std::get<1>(result)] 
				<< " '" 
				<< std::get<2>(result)
				<< "'" <<std::endl;
}
