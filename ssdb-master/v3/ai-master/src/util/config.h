 
#ifndef UTIL__CONFIG_H_
#define UTIL__CONFIG_H_

 

#include <string>
#include <vector>
#include <stdint.h>

#define CONFIG_MAX_LINE		4096

 
class Config{
	private:
		Config *parent;
		int depth;

		Config* build_key_path(const char *key);
		Config* add_child(const char *key, const char *val="", int lineno=0);
		const Config* find_child(const char *key) const;
	public:
		Config(const char *key=NULL, const char *val=NULL);
		~Config();

		static Config* load(const char *filename);
		int save(FILE *fp) const;
		int save(const char *filename) const;

		std::vector<Config *> children;
		std::string key;
		std::string val;

		Config* set(const char *key, const char *val);
		const Config* get(const char *key) const;
		int num() const;
		int get_num(const char *key) const;
		int64_t get_int64(const char *key) const;
		const char* str() const;
		const char* get_str(const char *key) const;

		bool is_comment() const{
			return key[0] == '#';
		}
		std::string ToString() const{
			return key + ": " + val;
		}
};

#endif

 
