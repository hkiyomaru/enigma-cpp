/**
 * @brief エニグマ（暗号機）のシミュレータ
 * @author Hirokazu Kiyomaru
 * @attention g++ -std=c++11 としてコンパイル
 * @date 2015/07/04
 * @file enigma.cpp
 */

//C++の標準ライブラリ
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include <random>
#include <algorithm>
#include <cctype>
#include <boost/algorithm/string.hpp>

//オプションの判定に用いる定数
#define BIT(num) ((unsigned int)1 << (num))
#define NORMAL_MODE 0                       //(0000 0000 0000 0000)
#define SHOW_TRANSITION_MODE BIT(0)         //(0000 0000 0000 0001)
#define SHOW_DEFAULT_KEY_ARRAY_MODE BIT(1)  //(0000 0000 0000 0010)
#define SHOW_KEY_ARRAY_MODE BIT(2)          //(0000 0000 0000 0100)
#define READ_FILE_MODE BIT(3)               //(0000 0000 0000 1000)
#define OUT_FILE_MODE BIT(4)                //(0000 0000 0001 0000)

//コピーコンストラクタと=演算子関数を無効にするためのマクロ
#define DISALLOW_COPY_AND_ASSIGN(Typename)		\
  Typename(const Typename&);					\
  void operator=(const Typename&)

//関数オブジェクトの定義
struct ToUpper {
  char operator()(char c){
	return toupper(c);
  }
};
struct IsDigit {
  char operator()(char c){
	return isdigit(c);
  }
};

//プロトタイプ宣言
std::map<char, int> Alpha2AlphaID();
std::map<int, char> AlphaID2Alpha();
[[noreturn]] void ShowUsage();

/**
 * @class  Arguments
 * @brief コマンドライン引数の情報を格納
 */
class Arguments{
private:
  std::string key_;           //エニグマのキーを格納するための変数(default:AAA)
  std::string code_;          //暗号化（平文化）する文を格納するための変数
  std::string in_file_name_;  //入力ファイル名を格納するための変数
  std::string out_file_name_; //出力ファイル名を格納するための変数
  unsigned int mode_;         //オプションを格納するための変数
  DISALLOW_COPY_AND_ASSIGN(Arguments);
public:
  /**
   * デフォルトコンストラクタ
   */
  Arguments(){
	key_ = "OOO";
	code_ = "";
	in_file_name_ = "";
	out_file_name_ = "";
	mode_ = NORMAL_MODE;
  }
        
  /**
   * デストラクタ
   */
  ~Arguments(){
  }
        
  /**
   * @brief key_に対するgetアクセサ
   * @param なし
   * @return key_の値
   */
  inline std::string getKey() const{
	return key_;
  }
        
  /**
   * @brief key_に対するsetアクセサ
   * @param [in] key key_にセットする値
   * @return なし
   */
  inline void setKey(const std::string key){
	key_ = key;
  }
        
  /**
   * @brief code_に対するgetアクセサ
   * @param なし
   * @return code_の値
   */
  inline std::string getCode() const{
	return code_;
  }
        
  /**
   * @brief code_に対するsetアクセサ
   * @param [in] code code_にセットする値
   * @return なし
   */
  inline void setCode(const std::string code){
	code_ = code;
  }
        
  /**
   * @brief in_file_name_に対するgetアクセサ
   * @param なし
   * @return in_file_name_の値
   */
  inline std::string getInFileName() const{
	return in_file_name_;
  }
        
  /**
   * @brief in_file_name_に対するsetアクセサ
   * @param [in] in_file_name in_file_name_にセットする値
   * @return なし
   */
  inline void setInFileName(const std::string in_file_name){
	in_file_name_ = in_file_name;
  }
        
  /**
   * @brief out_file_name_に対するgetアクセサ
   * @param なし
   * @return out_file_name_の値
   */
  inline std::string getOutFileName() const{
	return out_file_name_;
  }
        
  /**
   * @brief out_file_name_に対するsetアクセサ
   * @param [in] out_file_name out_file_name_にセットする値
   * @return なし
   */
  inline void setOutFileName(const std::string out_file_name){
	out_file_name_ = out_file_name;
  }
        
  /**
   * @brief mode_に対するgetアクセサ
   * @param なし
   * @return mode_の値
   */
  inline unsigned int getMode() const{
	return mode_;
  }
        
  /**
   * @brief mode_に対するsetアクセサ
   * @param [in] mode mode_にセットする値
   * @return なし
   */
  inline void setMode(const unsigned int mode){
	mode_ = mode;
  }
};

/**
 * @class  Plugboard
 * @brief エニグマのプラグボード部分を実装
 */
class Plugboard{
private:
  std::vector<int> plugboard; //プラグボードのキー配列
  DISALLOW_COPY_AND_ASSIGN(Plugboard);
public:
  /**
   * デフォルトコンストラクタ
   */
  Plugboard(){
	for(int i = 0; i < 26; i++){
	  plugboard.push_back(i);
	}
  }
  /**
   * コンストラクタ
   * @param [in] キー配列を初期化する乱数の種
   */
  explicit Plugboard(const unsigned int seed){
	for(int i = 0; i < 26; i++){
	  plugboard.push_back(i);
	}
	srand(seed);
	std::random_shuffle(plugboard.begin(), plugboard.end());
  }
        
  /**
   * @brief 暗号化を行う(行き)
   * @param [in] code アルファベットのID
   * @return 換字されたアルファベットのID
   */
  inline const int GoingEncipher(const int code){
	int code_ = plugboard[code];
	return code_;
  }
        
  /**
   * @brief 暗号化を行う(帰り)
   * @param [in] code アルファベットのID
   * @return 換字されたアルファベットのID
   */
  int ReturningEncipher(const int code) const{
	int code_ = 0;
	for(unsigned int i=0; i<plugboard.size(); i++){
	  if(plugboard[i] == code){
		code_ = i;
		break;
	  }
	}
	return code_;
  }
        
  /**
   * @brief 暗号化と変換経過の表示を行う(行き)
   * @param [in] code アルファベットのID
   * @return 換字されたアルファベットのID
   */
  inline int VisibleGoingEncipher(const int code) const{
	int code_ = code;
	std::map<int, char> alphaIDmap = AlphaID2Alpha();
	std::cout << "\t  " << alphaIDmap[code_] << " --> ";
	code_ = plugboard[code_];
	std::cout << alphaIDmap[code_] << " --> ";
	return code_;
  }
        
  /**
   * @brief 暗号化と変換経過の表示を行う(帰り)
   * @param [in] code アルファベットのID
   * @return 換字されたアルファベットのID
   */
  int VisibleReturningEncipher(const int code) const{
	std::map<int, char> alphaIDmap = AlphaID2Alpha();
	int code_ = 0;
	for(unsigned int i=0; i<plugboard.size(); i++){
	  if(plugboard[i] == code){
		code_ = i;
		break;
	  }
	}
	std::cout << alphaIDmap[code_] << std::endl;
	return code_;
  }
        
  /**
   * @brief キー配列を表示する
   * @param なし
   * @return なし
   */
  void ShowKeyArray() const{
	int tmp;
	std::map<int, char> alphaIDmap = AlphaID2Alpha();
	std::cout << "\t  Plugboard [ " ;
	for(unsigned int i=0; i<plugboard.size(); i++){
	  tmp = plugboard[i];
	  std::cout << alphaIDmap[tmp] << " ";
	}
	std::cout << "]" << std::endl;
  }
};

/**
 * @class Scrambler
 * @brief エニグマのスクランブラー（歯車）を実装
 */
class Scrambler{
private:
  DISALLOW_COPY_AND_ASSIGN(Scrambler);
protected:
  std::vector<int> rotor; //スクランブラーのキー配列
public:
  /**
   * デフォルトコンストラクタ
   */
  Scrambler(){
	for(int i = 0; i < 26; i++){
	  rotor.push_back(i);
	}
  }
        
  /**
   * コンストラクタ
   * @param [in] キー配列を初期化する乱数の種
   */
  explicit Scrambler(const unsigned int seed){
	for(int i = 0; i < 26; i++){
	  rotor.push_back(i);
	}
	srand(seed);
	std::random_shuffle(rotor.begin(), rotor.end());
  }
        
  /**
   * デストラクタ
   */
  virtual ~Scrambler(){
  }
        
  /**
   * @brief 指定の位置にスクランブラーのキーを合わせる
   * @param [in] key 合わせるキー
   * @return なし
   */
  void Set(const int key){
	while(rotor[0] != key){
	  rotor.insert(rotor.begin(), rotor.back());
	  rotor.pop_back();
	}
  }
        
  /**
   * @brief キーを１つずらす
   * @param なし
   * @return なし
   */
  virtual void ChangeKey(){
	//末尾の要素を先頭にinsertした後pop_back
	rotor.insert(rotor.begin(), rotor.back());
	rotor.pop_back();
  }
        
  /**
   * @brief 暗号化を行う(行き)
   * @param [in] code アルファベットのID
   * @return 換字されたアルファベットのID
   */
  inline int GoingEncipher(const int code) const{
	return rotor[code];
  }
        
  /**
   * @brief 暗号化を行う(帰り)
   * @param [in] code アルファベットのID
   * @return 換字されたアルファベットのID
   */
  int ReturningEncipher(const int code) const{
	int temp;
	for(unsigned int i=0; i<rotor.size(); i++){
	  if(rotor[i] == code){
		temp = i;
		break;
	  }
	}
	return temp;
  }
        
  /**
   * @brief 暗号化と変換の経過表示を行う(行き)
   * @param [in] code アルファベットのID
   * @return 換字されたアルファベットのID
   */
  inline int VisibleGoingEncipher(const int code) const{
	int code_ = rotor[code];
	std::map<int, char> alphaIDmap = AlphaID2Alpha();
	std::cout << alphaIDmap[code_] << " --> ";
	return code_;
  }
        
  /**
   * @brief 暗号化と変換の経過表示を行う(帰り)
   * @param [in] code アルファベットのID
   * @return 換字されたアルファベットのID
   */
  int VisibleReturningEncipher(const int code) const{
	int code_ = 0;
	std::map<int, char> alphaIDmap = AlphaID2Alpha();
	for(unsigned int i=0; i<rotor.size(); i++){
	  if(rotor[i] == code){
		code_ = i;
		break;
	  }
	}
	std::cout << alphaIDmap[code_] << " --> ";
	return code_;
  }
        
  /**
   * @brief キー配列を表示する
   * @param なし
   * @return なし
   */
  void ShowKeyArray() const{
	int tmp;
	std::map<int, char> alphaIDmap = AlphaID2Alpha();
	std::cout << "[ " ;
	for(unsigned int i=0; i<rotor.size(); i++){
	  tmp = rotor[i];
	  std::cout << alphaIDmap[tmp] << " ";
	}
	std::cout << "]" << std::endl;
  }
};

/**
 * @class LatchingScrambler
 * @brief 次のスクランブラーを１目盛り回転させる機能を持ったスクランブラーを実装
 */
class LatchingScrambler : public Scrambler{
private:
  int cnt = 0;    //自分が回った回数をカウントするための変数
  Scrambler *nextRing = NULL; //自分が一周したときに回すスクランブラーの参照
  DISALLOW_COPY_AND_ASSIGN(LatchingScrambler);
public:
  /**
   * デフォルトコンストラクタ
   */
  LatchingScrambler(){
	cnt = 0;
  }
  /**
   * コンストラクタ
   * @param [in] 自分の次のリングの参照
   * @param [in] キー配列を初期化する乱数の種
   */
  LatchingScrambler(Scrambler *nextScrambler, const unsigned int seed) : Scrambler(seed){
	cnt = 0;
	nextRing = nextScrambler;
  }
        
  /**
   * @brief キーを１つずらす
   * @param なし
   * @return なし
   */
  void ChangeKey(){
	//末尾の要素を先頭にinsertした後pop_backしカウントを増やす
	rotor.insert(rotor.begin(), rotor.back());
	rotor.pop_back();
	AddCnt();
  }
        
  /**
   * @brief 自身がどれだけ回ったかカウントする
   * @param なし
   * @return なし
   */
  void AddCnt(){
	cnt++;
	if(cnt == 26){
	  AddNextCnt();
	  cnt = 0;
	}
  }
        
  /**
   * @brief 自分が1回転したら次のスクランブラーのキーを１目盛り回す
   * @param なし
   * @return なし
   */
  void AddNextCnt(){
	nextRing->ChangeKey();
  }
};

/**
 * @class Reflector
 * @brief エニグマのリフレクターを実装
 */
class Reflector{
private:
  std::vector<int> reflector; //リフレクターのキー配列
  DISALLOW_COPY_AND_ASSIGN(Reflector);
public:
  /**
   * デフォルトコンストラクタ
   */
  Reflector(){
	for(int i = 0; i < 26; i++){
	  reflector.push_back(i);
	}
	for(int i = 0; i < 13; i++){
	  std::swap(reflector[i], reflector[25 - i]);
	}
  }
  /**
   * コンストラクタ
   * @param [in] キー配列を初期化する乱数の種
   * @detail 例えば入力Aが出力Bに変換されるなら,入力Bは出力Aに変換されるように初期化
   */
  explicit Reflector(const unsigned int seed){
	for(int i = 0; i < 26; i++){
	  reflector.push_back(i);
	}
	std::vector<int> ref_copy = reflector;
	srand(seed);
	random_shuffle(ref_copy.begin(), ref_copy.end());
	for(int i=0; i < 13; i++){
	  //swapで入出力の対応関係を保った初期化を行う
	  std::swap(reflector[(ref_copy[i])], reflector[(ref_copy[25-i])]);
	}
  }
        
  /**
   * @brief 暗号化を行う
   * @param [in] code アルファベットのID
   * @return 換字されたアルファベットのID
   */
  inline int Reflect(const int code) const{
	return reflector[code];
  }
        
  /**
   * @brief 暗号化と変換経過の表示を行う
   * @param [in] code アルファベットのID
   * @return 換字されたアルファベットのID
   */
  inline int VisibleReflect(const int code) const{
	int code_ = reflector[code];
	std::map<int, char> alphaIDmap = AlphaID2Alpha();
	std::cout << alphaIDmap[code_] << " --> ";
	return code_;
  }
        
  /**
   * @brief キー配列を表示する
   * @param なし
   * @return なし
   */
  void ShowKeyArray() const{
	int tmp;
	std::map<int, char> alphaIDmap = AlphaID2Alpha();
	std::cout << "\t  Reflector [ " ;
	for(unsigned int i=0; i<reflector.size(); i++){
	  tmp = reflector[i];
	  std::cout << alphaIDmap[tmp] << " ";
	}
	std::cout << "]" << std::endl;
  }
        
};

/**
 * @class RingSet
 * @brief スクランブラーを統括する
 */
class RingSet{
private:
  Scrambler *ring3 = NULL;
  Scrambler *ring2 = NULL;
  Scrambler *ring1 = NULL;
  DISALLOW_COPY_AND_ASSIGN(RingSet);
public:
  /**
   * デフォルトコンストラクタ
   */
  RingSet(){
	ring3 = new Scrambler(30);
	ring2 = new LatchingScrambler(ring3,20);
	ring1 = new LatchingScrambler(ring2,10);
  }
        
  /**
   * デストラクタ
   */
  ~RingSet(){
	delete ring3;
	delete ring2;
	delete ring1;
  }
        
  /**
   * @brief それぞれのリングのキーを合わせる
   * @param [in] keyset それぞれのリングのキーのID
   * @return なし
   */
  void KeySet(const std::vector<int> keyset){
	ring1->Set(keyset[0]);
	ring2->Set(keyset[1]);
	ring3->Set(keyset[2]);
  }
        
  /**
   * @brief ring1のキーの配置を変える
   * @param なし
   * @return なし
   */
  void EndCycle(){
	ring1->ChangeKey();
  }
        
  /**
   * @brief 暗号化を行う(行き)
   * @param [in] code アルファベットのID
   * @return 換字されたアルファベットのID
   */
  inline int GoingEncipher(const int code) const{
	int code_ = code;
	code_ = ring1->GoingEncipher(code_);
	code_ = ring2->GoingEncipher(code_);
	code_ = ring3->GoingEncipher(code_);
	return code_;
  }

  /**
   * @brief 暗号化を行う(帰り)
   * @param [in] code アルファベットのID
   * @return 換字されたアルファベットのID
   */
  inline int ReturningEncipher(const int code) const{
	int code_ = code;
	code_ = ring3->ReturningEncipher(code_);
	code_ = ring2->ReturningEncipher(code_);
	code_ = ring1->ReturningEncipher(code_);
	return code_;
  }
        
  /**
   * @brief 暗号化と変換の経過表示を行う(行き)
   * @param [in] code アルファベットのID
   * @return 換字されたアルファベットのID
   */
  inline int VisibleGoingEncipher(const int code) const{
	int code_ = code;
	code_ = ring1->VisibleGoingEncipher(code_);
	code_ = ring2->VisibleGoingEncipher(code_);
	code_ = ring3->VisibleGoingEncipher(code_);
	return code_;
  }

  /**
   * @brief 暗号化と変換の経過表示を行う(帰り)
   * @param [in] code アルファベットのID
   * @return 換字されたアルファベットのID
   */
  inline int VisibleReturningEncipher(const int code) const{
	int code_ = code;
	code_ = ring3->VisibleReturningEncipher(code_);
	code_ = ring2->VisibleReturningEncipher(code_);
	code_ = ring1->VisibleReturningEncipher(code_);
	return code_;
  }
        
  /**
   * @brief キー配列を表示する
   * @param なし
   * @return なし
   */
  void ShowKeyArray() const{
	std::cout << "\t  Ring1     ";
	ring1->ShowKeyArray();
	std::cout << "\t  Ring2     ";
	ring2->ShowKeyArray();
	std::cout << "\t  Ring3     ";
	ring3->ShowKeyArray();
  }
};

/**
 * @class Enigma
 * @brief プログラムの中枢を実装
 */
class Enigma{
private:
  Plugboard *plugboard = NULL;
  RingSet *ringSet = NULL;
  Reflector *reflector = NULL;
  DISALLOW_COPY_AND_ASSIGN(Enigma);
public:
  /**
   * デフォルトコンストラクタ
   */
  Enigma(){
	plugboard = new Plugboard(100);
	ringSet = new RingSet();
	reflector = new Reflector(200);
  }
        
  /**
   * デストラクタ
   */
  ~Enigma(){
	delete plugboard;
	delete ringSet;
	delete reflector;
  }
        
  /**
   * それぞれのリングにキーを設定する
   * @param [in] key キーが大文字アルファベット3文字で与えられる
   * @return なし
   */
  void KeySet(const std::string key){
	std::map<char, int> alphamap = Alpha2AlphaID();
	/*keyを対応表に則ってint型に変更する*/
	std::vector<int> key_temp;
	for(unsigned int i = 0; i<key.length(); i++){
	  key_temp.push_back(alphamap[(key[i])]);
	}
	/*リングセットクラスのセット関数を呼び出してキーをセットする*/
	ringSet->KeySet(key_temp);
  }
        
  /**
   * 暗号化(複号化)を行う
   * @param [in] code この入力に対してEnigmaを実行する
   * @return Enigmaによる変換後の文字列
   */
  std::string Encryption(const std::string code) const{
	std::map<char, int> alphamap = Alpha2AlphaID();
	std::map<int, char> alphaIDmap = AlphaID2Alpha();
	std::string cryptogram = "";
            
	/*keyを対応表に則ってint型に変更する*/
	std::vector<int> code_temp;
	for(unsigned int i = 0; i<code.length(); i++){
	  code_temp.push_back(alphamap[(code[i])]);
	}

	/*一文字ずつ暗号化（複号化）を行う*/
	int temp = 0;
	for(unsigned int i=0; i<code_temp.size(); i++){
	  temp = code_temp[i];
	  temp = plugboard->GoingEncipher(temp);
	  temp = ringSet->GoingEncipher(temp);
	  temp = reflector->Reflect(temp);
	  temp = ringSet->ReturningEncipher(temp);
	  temp = plugboard->ReturningEncipher(temp);
                
	  cryptogram += alphaIDmap[temp];
	  ringSet->EndCycle();
	}
	return cryptogram;
  }
        
  /**
   * 暗号化(複号化)と変換経過の表示を行う
   * @param [in] code この入力に対してEnigmaを実行する
   * @return Enigmaによる変換後の文字列
   */
  std::string VisibleEncryption(const std::string code) const{
	std::map<char, int> alphamap = Alpha2AlphaID();
	std::map<int, char> alphaIDmap = AlphaID2Alpha();
	std::string cryptogram = "";
            
	/*keyを対応表に則ってint型に変更する*/
	std::vector<int> code_temp;
	for(unsigned int i = 0; i<code.length(); i++){
	  code_temp.push_back(alphamap[(code[i])]);
	}

	/*一文字ずつ暗号化（複号化）と変換経過の表示を行う*/
	std::cout << "\tCode Conversion Process\n";
	std::cout << "\t    Plg   Ri1   Ri2   Ri3   Ref   Ri3   Ri2   Ri1   Plg\n";
	int temp = 0;
	for(unsigned int i=0; i<code_temp.size(); i++){
	  temp = code_temp[i];
	  temp = plugboard->VisibleGoingEncipher(temp);
	  temp = ringSet->VisibleGoingEncipher(temp);
	  temp = reflector->VisibleReflect(temp);
	  temp = ringSet->VisibleReturningEncipher(temp);
	  temp = plugboard->VisibleReturningEncipher(temp);
                
	  cryptogram += alphaIDmap[temp];
	  ringSet->EndCycle();
	}
	std::cout << std::endl;
	return cryptogram;
  }
        
  /**
   * 暗号化(複号化)と毎回のキー配列・変換経過の表示を行う
   * @param [in] code この入力に対してEnigmaを実行する
   * @return Enigmaによる変換後の文字列
   */
  std::string KeyVisibleEncryption(const std::string code) const{
	std::map<char, int> alphamap = Alpha2AlphaID();
	std::map<int, char> alphaIDmap = AlphaID2Alpha();
	std::string cryptogram = "";
            
	/*keyを対応表に則ってint型に変更する*/
	std::vector<int> code_temp;
	for(unsigned int i = 0; i<code.length(); i++){
	  code_temp.push_back(alphamap[(code[i])]);
	}

	/*一文字ずつ暗号化（複号化）を行い、サイクル毎にキー配列を表示*/
	int temp = 0;
	for(unsigned int i=0; i<code_temp.size(); i++){
	  std::cout << "\tKey Array : " << (i+1) << "cycle\n";
	  ShowKeyArray();
	  std::cout << "\n";
	  std::cout << "\tCode Conversion Process\n";
	  std::cout << "\t    Plg   Ri1   Ri2   Ri3   Ref   Ri3   Ri2   Ri1   Plg\n";
	  temp = code_temp[i];
	  //std::cout << "\t  " << alphaIDmap[temp] << " --> ";
	  temp = plugboard->VisibleGoingEncipher(temp);
	  temp = ringSet->VisibleGoingEncipher(temp);
	  temp = reflector->VisibleReflect(temp);
	  temp = ringSet->VisibleReturningEncipher(temp);
	  temp = plugboard->VisibleReturningEncipher(temp);
                
	  cryptogram += alphaIDmap[temp];
	  ringSet->EndCycle();
	  std::cout << "\n";
	}
	std::cout << std::endl;
	return cryptogram;
  }
        
  /**
   * @brief キー配列を表示する
   * @param なし
   * @return なし
   */
  void ShowKeyArray() const{
	std::cout << "\t            [ A B C D E F G H I J K L M N O P Q R S T U V W X Y Z ]" << std::endl;
	std::cout << "\t              | | | | | | | | | | | | | | | | | | | | | | | | | |  " << std::endl;
	plugboard->ShowKeyArray();
	ringSet->ShowKeyArray();
	reflector->ShowKeyArray();
  }
        
  /**
   * モードに応じた処理を実行する
   * @param [in] arguments 引数情報を格納しているオブジェクト
   * @return Enigmaによる変換後の文字列
   */
  std::string Execute(const Arguments &arguments) const{
	std::string code = arguments.getCode();
	unsigned int mode = arguments.getMode();
	if(mode & OUT_FILE_MODE){
	  std::ofstream ofs(arguments.getOutFileName());
	  std::string cryptogram = Encryption(code);
	  ofs << cryptogram << std::endl;
	  return "";
	}
	if(mode & SHOW_DEFAULT_KEY_ARRAY_MODE){
	  std::cout << "\tDefault Key Array\n";
	  ShowKeyArray();
	  std::cout << std::endl;
	}
	if(mode & SHOW_KEY_ARRAY_MODE){
	  return KeyVisibleEncryption(code);    
	}else if(mode & SHOW_TRANSITION_MODE){
	  return VisibleEncryption(code);
	}else{
	  return Encryption(code);
	}
  }
};

/**
 * プロトタイプ宣言
 */
int GetOption(int argc, char *argv[], Arguments &arguments);

/**
 * @brief プログラムのエントリポイント
 * @param [in] argc コマンドライン引数の数
 * @param [in] argv コマンドライン引数
 * @return 終了ステータス
 */
int main(int argc, char *argv[]){
  /*変数宣言*/
  Arguments arguments; //引数を格納するためのオブジェクト
  std::string cryptogram = "";  //暗号文（平文）を格納するための変数
  Enigma *enigma = new Enigma();  //エニグマのオブジェクト
    
  /*引数がなかったときの処理*/
  if(argc == 1){
	std::cerr << "\tInvalid call.\n";
	std::cerr << "\tThe option to show help is \'-h\'.\n";
	std::cerr << "\tProgram stopped." << std::endl;
	return -1;
  }
    
  /*オプションの解析*/
  if((GetOption(argc, argv, arguments)) < 0){
	std::cerr << "\tProgram stopped." << std::endl;
	return -1;
  };
    
  /*エニグマのキーをセット*/
  enigma->KeySet(arguments.getKey());
    
  /*エニグマの実行*/
  cryptogram = enigma->Execute(arguments);
    
  /*結果出力*/
  std::cout << "\tArgument Information\n";
  if(arguments.getMode() & READ_FILE_MODE){
	std::cout << "\t  -Argument File -> " << arguments.getInFileName() << "\n";
  }else{
	std::cout << "\t  -Argument String -> " << arguments.getCode() << "\n";
  }
  std::cout << "\t  -Key Setting -> " << arguments.getKey() << "\n" << std::endl;;
  std::cout << "\tConversion Result\n";
  if(arguments.getMode() & OUT_FILE_MODE){
	std::cout << "\t  -Encrypted File -> " << arguments.getOutFileName() << std::endl;
  }else{
	std::cout << "\t  -Encrypted String -> " << cryptogram << std::endl;
  }
    
  delete enigma;
  return 0;
}


/**
 * @brief アルファベットを数字に対応付けるmapを生成する関数
 * @param なし
 * @return アルファベット->数字の対応表
 */
std::map<char, int> Alpha2AlphaID(){
  std::map<char, int> alphaTable;
  const std::string ALPHA = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  for(unsigned int i=0; i<ALPHA.length(); i++){
	alphaTable[ALPHA[i]] = i;
  }
  return alphaTable;
}


/**
 * @brief 数字をアルファベットに対応付けるmapを生成する関数
 * @param なし
 * @return 数字->アルファベットの対応表
 */
std::map<int, char> AlphaID2Alpha(){
  std::map<int, char> alphaTable;
  const std::string ALPHA = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  for(unsigned int i=0; i<ALPHA.length(); i++){
	alphaTable[i] = ALPHA[i];
  }
  return alphaTable;
}


/**
 * @brief オプションを解析する関数
 * @param [in] argc コマンドライン引数の数
 * @param [in] argv コマンドライン引数
 * @param [out] arguments 引数情報を格納するクラスのオブジェクト
 * @return 終了ステータス
 */
int GetOption(int argc, char *argv[], Arguments &arguments){
  int ch = 0;
  std::string key = arguments.getKey();
  std::string code = arguments.getCode();
  std::string in_file_name = arguments.getInFileName();
  std::string out_file_name = arguments.getOutFileName();
  unsigned int mode = arguments.getMode();
    
  /*オプションを解析*/
  while((ch = getopt(argc, argv, "s:htdkf:o:")) != -1){
	switch(ch){
	case 's':   //スクランブラーをセット
	  key = optarg;
	  transform(key.begin(), key.end(), key.begin(), ToUpper());
	  /*キーがアルファベット３文字でない場合エラー処理*/
	  if((any_of(key.begin(), key.end(), IsDigit())) || key.length() != 3){
		std::cerr << "\t\"" << key << "\" is invalid key! Input three characters like \"AAA\"" << std::endl;
		return -1;
	  }
	  break;
	case 'h':   //ヘルプ
	  ShowUsage();
	  break;
	case 't':
	  mode |= SHOW_TRANSITION_MODE;
	  break;
	case 'd':
	  mode |= SHOW_DEFAULT_KEY_ARRAY_MODE;
	  break;
	case 'k':
	  mode |= SHOW_KEY_ARRAY_MODE;
	  break;
	case 'f':
	  mode |= READ_FILE_MODE;
	  in_file_name = optarg;
	  break;
	case 'o':
	  mode |= OUT_FILE_MODE;
	  out_file_name = optarg;
	  break;
	default:
	  std::cerr << "\tInvalid option was riquired!" << std::endl;
	  return -1;
	  break;
	}
  }
    
  /*引数を格納*/
  if(mode & READ_FILE_MODE){  //テキストファイル変換モードの時
	std::ifstream ifs(in_file_name);
	if(ifs.fail()){
	  std::cerr << "\tFile cannot open. > " << in_file_name << std::endl;
	  return -1;
	}
	std::string buf = "";
	std::vector<std::string> split_buf;
	while(getline(ifs, buf)){
	  boost::algorithm::split(split_buf, buf, boost::is_any_of(" "));
	  for(unsigned int i = 0; i<split_buf.size(); i++){
		code += split_buf[i];
	  }
	}
	ifs.close();
  }else{  //コマンドライン引数変換モードのとき
	for(;optind<argc; optind++){
	  code += argv[optind];
	}
  }
    
  /*数字が含まれている場合エラー処理*/
  if(none_of(code.begin(), code.end(), IsDigit())){
	/*アルファベットはすべて大文字に変換*/
	transform(code.begin(), code.end(), code.begin(), ToUpper());
  }else{
	std::cerr << "\tArguments should be letters!" << std::endl;
	return -1;
  }
    
  /*Argumentsオブジェクトにそれぞれの情報を格納*/
  arguments.setKey(key);
  arguments.setCode(code);
  arguments.setMode(mode);
  arguments.setInFileName(in_file_name);
  arguments.setOutFileName(out_file_name);
  return 0;
}


/**
 * @brief 使い方を表示し、プログラムを終了する
 * @param なし
 * @return なし
 */
[[noreturn]] void ShowUsage(){
  printf("\t[Usage]\n");
  printf("\t  *** Arguments should be string. ***\n");
  printf("\t  attention : 1.Spaces are filled.\n");
  printf("\t              2.You can only use alphabetic characters.\n");
  printf("\t  option -> -s : You can set Scrambler.\te.g. -s \"ABC\"\n");
  printf("\t            -t : You can show process of conversion.\n");
  printf("\t            -d : You can show default key arrays of all parts.\n");
  printf("\t            -k : You can show transition of key arrays and process of conversion.\n");
  printf("\t            -f : You can select an input text file.\n");
  printf("\t            -o : You can set an output text file.\n");
  printf("\t            -h : You can show help.\n");
  exit(0);
}
