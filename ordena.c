#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#pragma pack(1)

typedef struct _Endereco Endereco;

struct _Endereco
{
	char logradouro[72];
	char bairro[72];
	char cidade[72];
	char uf[72];
	char sigla[2];
	char cep[8];
	char lixo[2];
};

int compara(const void *e1, const void *e2)
{
	return strncmp(((Endereco *)e1)->cep, ((Endereco *)e2)->cep, 8);
}

int intercala(char *file1, char *file2, int prox)
{
	FILE *a, *b, *saida;
	Endereco ea, eb;

	char file3[10];
	sprintf(file3, "cep_%d.dat", prox);

	a = fopen(file1, "r");
	b = fopen(file2, "r");
	saida = fopen(file3, "w");

	fread(&ea, sizeof(Endereco), 1, a);
	fread(&eb, sizeof(Endereco), 1, b);

	while (!feof(a) && !feof(b))
	{
		if (compara(&ea, &eb) < 0)
		{
			fwrite(&ea, sizeof(Endereco), 1, saida);
			fread(&ea, sizeof(Endereco), 1, a);
		}
		else
		{
			fwrite(&eb, sizeof(Endereco), 1, saida);
			fread(&eb, sizeof(Endereco), 1, b);
		}
	}

	while (!feof(a))
	{
		fwrite(&ea, sizeof(Endereco), 1, saida);
		fread(&ea, sizeof(Endereco), 1, a);
	}

	while (!feof(b))
	{
		fwrite(&eb, sizeof(Endereco), 1, saida);
		fread(&eb, sizeof(Endereco), 1, b);
	}

	fclose(a);
	remove(file1);
	fclose(b);
	remove(file2);
	fclose(saida);
}

int main(int argc, char**argv)
{

	if(argc != 2)
	{
		fprintf(stderr, "USO: %s [NOME DO ARQUIVO]", argv[0]);
		return 1;
	}

	clock_t begin = clock();

	FILE *f, *saida;
	Endereco *e;
	long posicao, qtd, parte;
	int nPartes = 8;

	f = fopen(argv[1], "r");
	fseek(f, 0, SEEK_END);
	posicao = ftell(f);
	qtd = posicao / sizeof(Endereco);
	parte = qtd / nPartes;
	rewind(f);

	for (int i = 1; i <= nPartes; i++)
	{
		char fileName[11];
		if (i == nPartes)
		{
			parte += qtd % nPartes;
		}

		e = (Endereco *)malloc(parte * sizeof(Endereco));

		if (fread(e, sizeof(Endereco), parte, f) == parte)
		{
			printf("Parte %d lido = OK\n", i);
		}
		qsort(e, parte, sizeof(Endereco), compara);
		printf("Parte %d ordenado = OK\n", i);

		sprintf(fileName, "cep_%d.dat", i);

		saida = fopen(fileName, "w");
		fwrite(e, sizeof(Endereco), parte, saida);
		fclose(saida);
		printf("Parte %d escrita = OK\n", i);
		free(e);
	}

	fclose(f);

	int atual = 1;
	int prox = nPartes + 1;

	while (atual < prox - 1)
	{
		char file1[11], file2[11];
		sprintf(file1, "cep_%d.dat", atual);
		sprintf(file2, "cep_%d.dat", atual + 1);
		intercala(file1, file2, prox);
		atual+=2;
		prox++;
	}

	char final[11];
	sprintf(final, "cep_%d.dat", prox-1);
	rename(final, "cep_ordenado.dat");
	printf("Arquivo ordenado com sucesso!!");

	clock_t end = clock();
	double time_spent = (double)(end - begin)/CLOCKS_PER_SEC;

	printf("\nTempo de excucao: %.2f segundos", time_spent);

	return 0;
}
