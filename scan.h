#ifndef _SCAN_H_
#define _SCAN_H_

/* tamanho maximo de um token */
#define MAXTOKENLEN 40

/* armazena o lexema de cada token */
extern char tokenString[MAXTOKENLEN+1];

/* retorna o proximo token no arquivo fonte */
TokenType getToken(void);

#endif
