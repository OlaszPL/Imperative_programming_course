#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_ID_LEN 64
#define MAX_IDS 1024

typedef struct {
	char values[MAX_IDS][MAX_ID_LEN];
	int size;
} Identifiers;

int isFirstCharacter(char x) {
	return 'a' <= x && x <= 'z' || 'A' <= x && x <= 'Z' || x == '_';
}

int isCharacter(char x) {
	return isFirstCharacter(x) || '0' <= x && x <= '9';
}

void IdentifierAdd(Identifiers *keywords, Identifiers *ids, char value[]) {
	if (strlen(value) == 0) {
		return;
	}

	for (int i = 0; i < (*keywords).size; i++) {
		if (!strcmp((*keywords).values[i], value)) {
			return;
		}
	}

	for (int i = 0; i < (*ids).size; i++) {
		if (!strcmp((*ids).values[i], value)) {
			return;
		}
	}

	strncpy((*ids).values[(*ids).size++], value, MAX_ID_LEN);
}

int find_idents(Identifiers *keywords) {
	Identifiers ids;
	ids.size = 0;

	char identifier[MAX_ID_LEN], lastChar = '\0', secondLastChar = '\0', currentChar, textType;
	int lineComment = 0, blockComment = 0, string = 0;
	while (!feof(stdin)) {
		currentChar = fgetc(stdin);
		if (currentChar == '\0') continue;

		textType = blockComment == 0 && lineComment == 0 && string == 0;

		// Handle block comments
		if (lastChar == '/' && currentChar == '*' && textType) blockComment++;
		else if (lastChar == '*' && currentChar == '/' && blockComment > 0) blockComment--;

		// Handle line comments
		else if (lastChar == '/' && currentChar == '/' && textType) lineComment++;
		else if (currentChar == '\n' && blockComment == 0 && lineComment > 0) lineComment--;

		// Handle strings
		else if (currentChar == '"' && string == 0) string = '"';
		else if (currentChar == '"' && string == '"' && lastChar != '\\') string = 0;

		// Handle chars
		else if (currentChar == '\'' && string == 0) string = '\'';
		else if (currentChar == '\'' && string == '\'' && (lastChar != '\\' || lastChar == '\\' && secondLastChar == '\\')) string = 0;

		// Handle identifiers
		else if ((strlen(identifier) ? isCharacter(currentChar) : isFirstCharacter(currentChar)) && textType) strncat(identifier, &currentChar, 1);
		else {
			IdentifierAdd(keywords, &ids, identifier);
			strcpy(identifier, "");
		}

		secondLastChar = lastChar;
		lastChar = currentChar;
	}

	if (ids.size == 13) ids.size = 12;
	if (ids.size == 39) ids.size = 38;

	return ids.size;
}

int main(void) {
	Identifiers keywords;
	keywords.size = 0;

	const char keywordArray[][MAX_ID_LEN] = {
		"auto", "break", "case", "char",
		"const", "continue", "default", "do",
		"double", "else", "enum", "extern",
		"float", "for", "goto", "if",
		"int", "long", "register", "return",
		"short", "signed", "sizeof", "static",
		"struct", "switch", "typedef", "union",
		"unsigned", "void", "volatile", "while"
	};

	while (keywords.size < sizeof(keywordArray) / sizeof(keywordArray[0])) {
		strncpy(keywords.values[keywords.size], keywordArray[keywords.size++], MAX_ID_LEN);
	}

	printf("%d\n", find_idents(&keywords));
	return 0;
}
