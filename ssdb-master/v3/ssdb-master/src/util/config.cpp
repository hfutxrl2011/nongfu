/**
* @file config.cpp
* @Brief config manager
* @author Youzu-BigApp
* @version 1.0.0
* @date 2016-03-10
*/
#include "log.h"
#include "config.h"
#include "strings.h"
#include "exception.h"
#include "tools.h"

inline static
int is_kv_seperator(int ch){
	return (ch == '=') || (ch == ':');
}

Config* Config::load(const char *filename){
	FILE *fp = NULL;
	int lineno = 0;

	if(strcmp(filename, "stdout") == 0){
		fp = stdin;
	}else{
		fp = fopen(filename, "r");
		if(!fp){
			log_error("error opening file '%s': %s", filename, strerror(errno));
			return NULL;
		}
	}

	Config *root = new(std::nothrow) Config("root", "");
	ASSERT_SYS(NULL != root, "try to new Config failed, no memory");
	Config *cfg = root;
	int last_indent = 0;
	char buf[CONFIG_MAX_LINE];
	while(fgets(buf, sizeof(buf), fp)){
		lineno++;

		buf[strlen(buf) - 1] = '\0'; 
		if(is_empty_str(buf)){
			continue;
		}

		int indent = strspn(buf, "\t");
		char *key = buf + indent;

		if(*key == '#'){
			cfg->add_child("#", key + 1, lineno);
			continue;
		}
		if(indent <= last_indent){
			for(int i = indent; i <= last_indent; i++){
				if(cfg != root){
					cfg = cfg->parent;
				}
			}
		}else if(indent > last_indent + 1){
			log_error("invalid indent line(%d)", lineno);
			goto err;
		}
		
		if(isspace(*key)){
			log_error("invalid line(%d): unexpected whitespace char '%c'", lineno, *key);
			goto err;
		}

		char *val = key;
		while(*val && !is_kv_seperator(*val)){
			val++;
		}
		if(*val == '\0'){
			log_error("invalid line(%d): %s, expecting ':' or '='", lineno, *val);
			goto err;
		}else if(!is_kv_seperator(*val)){
			log_error("invalid line(%d): unexpected char '%c', expecting ':' or '='", lineno, *val);
			goto err;
		}
		*val++ = '\0';
		key = trim(key);
		val = trim(val);

		cfg = cfg->add_child(key, val, lineno);
		if(cfg == NULL){
			goto err;
		}

		last_indent = indent;
	}
	if(ferror(fp)){
		log_error("error while reading file %s", filename);
		goto err;
	}
	fclose(fp);
	return root;
err:
	SAFE_DELETE(root)
	if(fp && fp != stdin){
		fclose(fp);
	}
	return NULL;
}

Config::Config(const char *key, const char *val){
	this->parent = NULL;
	this->depth = 0;
	if(key){
		this->key = key;
	}
	if(val){
		this->val = val;
	}
};

Config::~Config(){
	for(int i = 0; i < (int)children.size(); i++){
		SAFE_DELETE(children[i]);
	}
}

Config* Config::build_key_path(const char *key){
	char path[CONFIG_MAX_LINE];
	Config *conf = this;
	Config *c;

	snprintf(path, CONFIG_MAX_LINE, "%s", key);

	char *f, *fs; 
	f = fs = path;
	while(1){
		switch(*fs++){
			case '.':
			case '/':
				*(fs - 1) = '\0';
				c = (Config *)conf->find_child(f);
				if(c == NULL){
					c = conf->add_child(f);
				}
				conf = c;
				f = fs;
				break;
			case '\0':
				c = (Config *)conf->find_child(f);
				if(c == NULL){
					c = conf->add_child(f);
				}
				return c;
			default:
				break;
		}
	}
}

Config* Config::set(const char *key, const char *val){
	Config *c = this->build_key_path(key);
	c->val = val;
	log_trace("%*s'%s' : '%s'", depth*4, "", this->key.c_str(), key);
	return c;
}

Config* Config::add_child(const char *key, const char *val, int lineno){
	Config *c = new(std::nothrow) Config(key, val);
	ASSERT_SYS(NULL != c, "try to new Config failed, no memory");
	c->parent = this;
	c->depth  = this->depth + 1;
	children.push_back(c);
	return c;
}

const Config* Config::find_child(const char *key) const{
	int i = (int)children.size()-1;
	for(; i >= 0; i--){
		if(children[i]->key == key){
			return children[i];
		}
	}
	return NULL;
}

const Config* Config::get(const char *key) const{
	char path[CONFIG_MAX_LINE];
	const Config *conf = this;

	snprintf(path, CONFIG_MAX_LINE, "%s", key);

	char *f, *fs; /* field, field seperator */
	f = fs = path;
	while(conf){
		switch(*fs++){
			case '.':
			case '/':
				*(fs - 1) = '\0';
				conf = conf->find_child(f);
				f = fs;
				break;
			case '\0':
				conf = conf->find_child(f);
				return conf;
			default:
				break;
		}
	}
	return conf;
}

int Config::num() const{
	return atoi(this->val.c_str());
}

const char* Config::str() const{
	return this->val.c_str();
}

int Config::get_num(const char *key) const{
	const Config *c = this->get(key);
	if(!c){
		return 0;
	}
	return c->num();
}

int64_t Config::get_int64(const char *key) const{
	const Config *c = this->get(key);
	if(!c){
		return 0;
	}
	return str_to_int64(c->val);
}

const char* Config::get_str(const char *key) const{
	const Config *c = this->get(key);
	if(!c){
		return "";
	}
	return c->str();
}

int Config::save(FILE *fp) const{
	for(int i = 0; i < (int)children.size(); i++){
		Config *c = children[i];
		for(int j=0; j<this->depth; j++){
			fputc('\t', fp);
		}

		if(c->is_comment()){
			fprintf(fp, "#%s\n", c->val.c_str());
		}else{
			fprintf(fp, "%s: %s\n", c->key.c_str(), c->val.c_str());
		}
		c->save(fp);
	}
	return 0;
}

int Config::save(const char *filename) const{
	FILE *fp;

	if(strcmp(filename, "stdout") == 0){
		fp = stdout;
	}else if(strcmp(filename, "stderr") == 0){
		fp = stderr;
	}else{
		fp = fopen(filename, "w");
		if(!fp){
			log_error("error opening file '%s': %s", filename, strerror(errno));
			return -1;
		}
	}
	this->save(fp);
	if(fp && fp != stdout && fp != stderr){
		fclose(fp);
	}
	return 0;
}

