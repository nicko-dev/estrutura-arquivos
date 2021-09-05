#include <stdio.h>
#include <string.h>


typedef struct _Endereco Endereco;

struct _Endereco
{
	char logradouro[72];
	char bairro[72];
	char cidade[72];
	char uf[72];
	char sigla[2];
	char cep[8];
	char lixo[2]; // Ao Espaço no final da linha + quebra de linha
};

int main(int argc, char**argv)
{
	FILE *f;
	Endereco e;

	if(argc != 2)
	{
		fprintf(stderr, "USO: %s [CEP]", argv[0]);
		return 1;
	}

	f = fopen("cep_ordenado.dat","r");

	int start = 0, middle, i=0;

	fseek(f,0,SEEK_END);
	int end = ftell(f)/sizeof(Endereco) -1;

	int tentativas = 0;
	int found = 0; 

	while(start <= end){
		i++;
		middle = (start + end)/2;
		fseek(f, middle*sizeof(Endereco), SEEK_SET);
		fread(&e,sizeof(Endereco),1,f);
		
		if(strncmp(argv[1], e.cep, 8) == 0){
			found = 1;
			printf("%s\nAchei o cep após %d tentativas",e, i);
			return 0;
		}else if(strncmp(argv[1], e.cep, 8) > 0){
			start = middle + 1;
		}else{
			end = middle - 1;
		}
	}

	printf("CEP não encontrado após %d tentativas", i);

	fclose(f);
	return 0;
}