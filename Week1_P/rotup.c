#include <unistd.h>
#include <stdio.h>

int main(int argc, char** argv){
        char in[128];
	ssize_t len = read(0, in, 128);
	in[len-1] = '\0';

	char rot13[len+1];
	int i=0;
	char c = in[i];
	while((c>='A' && c<='Z') || (c>='a' && c<='z')){
		if(c>='a' && c<='z'){
			c = c-'a'+'A';
		}
		rot13[i] = (((c-'A')+13)%26)+'A';
		i++;
		c = in[i];
	}
	rot13[i] = '\0';
	printf("Hallo: %s -- %s\n", in, rot13);
}
