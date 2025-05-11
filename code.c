#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_SENTENCE_LENGTH 300
#define MAX_SENTENCE_COUNT 200

int readSentencesFromFile(char *fileName, char *sentences[], int maxSentenceCount);

char **findUniqueWords(char *sentences1[], int sentenceCount1, char *sentences2[], int sentenceCount2, int *uniqueWordCount);

int **createOneHotVectors(char *sentences1[], char *sentences2[], int sentenceCount1, int sentenceCount2, char **uniqueWords, int uniqueWordCount);

int main()
{
    char class1File[] = "pozitif.txt";
    char class2File[] = "negatif.txt";
    char *sentences_1[MAX_SENTENCE_COUNT];
    char *sentences_2[MAX_SENTENCE_COUNT];
    int sentenceCount_1 = 0, sentenceCount_2 = 0;
    int i = 0;
    int j = 0;
    int uniqueWordCount = 0;

    sentenceCount_1 = readSentencesFromFile(class1File, sentences_1, MAX_SENTENCE_COUNT);
    sentenceCount_2 = readSentencesFromFile(class2File, sentences_2, MAX_SENTENCE_COUNT);

    char **uniqueWords = findUniqueWords(sentences_1, sentenceCount_1, sentences_2, sentenceCount_2, &uniqueWordCount);

    int **oneHotVectors = createOneHotVectors(sentences_1, sentences_2, sentenceCount_1, sentenceCount_2, uniqueWords, uniqueWordCount);

    printf("One hot vectors:\n");
    for (i = 0; i < sentenceCount_1 + sentenceCount_2; i++)
    {
        printf("%d. sentence one hot vector:\n ", i + 1);

        for (j = 0; j < uniqueWordCount; j++)
        {
            printf("%d ", oneHotVectors[i][j]);
        }
        printf("\n\n");
    }

    for (i = 0; i < sentenceCount_1; i++)
    {
        free(sentences_1[i]);
    }
    for (i = 0; i < sentenceCount_2; i++)
    {
        free(sentences_2[i]);
    }

    for (i = 0; i < uniqueWordCount; i++)
    {
        free(uniqueWords[i]);
    }
    free(uniqueWords);

    for (i = 0; i < sentenceCount_1 + sentenceCount_2; i++)
    {
        free(oneHotVectors[i]);
    }
    free(oneHotVectors);

    return 0;
}

int readSentencesFromFile(char *fileName, char *sentences[], int maxSentenceCount)
{
    FILE *file;
    char sentence[MAX_SENTENCE_LENGTH];
    int sentenceCount = 0;

    file = fopen(fileName, "r");
    if (file == NULL)
    {
        perror("Error: Could not open file");
        exit(EXIT_FAILURE);
    }

    while (fgets(sentence, MAX_SENTENCE_LENGTH, file) != NULL && sentenceCount < maxSentenceCount)
    {
        sentence[strcspn(sentence, "\n")] = '\0';
        sentences[sentenceCount] = strdup(sentence);
        sentenceCount++;
    }

    fclose(file);
    return sentenceCount;
}

char **findUniqueWords(char *sentences1[], int sentenceCount1, char *sentences2[], int sentenceCount2, int *uniqueWordCount)
{
    char **uniqueWords = NULL;
    int totalUniqueWordCount = 0;
    int i, j;

    for (i = 0; i < sentenceCount1; i++)
    {
        char *sentenceCopy1 = strdup(sentences1[i]);
        char *word = strtok(sentenceCopy1, " ");
        while (word != NULL)
        {
            size_t wordLength = strlen(word);
            if (wordLength > 0 && ispunct(word[wordLength - 1])) {
                word[wordLength - 1] = '\0';
            }

            int isUnique = 1;
            for (j = 0; j < totalUniqueWordCount; j++)
            {
                if (strcmp(word, uniqueWords[j]) == 0)
                {
                    isUnique = 0;
                    break;
                }
            }

            if (isUnique)
            {
                totalUniqueWordCount++;
                uniqueWords = realloc(uniqueWords, totalUniqueWordCount * sizeof(char *));
                if (uniqueWords == NULL) {
                    perror("Memory reallocation failed");
                    exit(EXIT_FAILURE);
                }
                uniqueWords[totalUniqueWordCount - 1] = strdup(word);
                if (uniqueWords[totalUniqueWordCount - 1] == NULL) {
                     perror("Memory allocation failed");
                    exit(EXIT_FAILURE);
                }
            }
            word = strtok(NULL, " ");
        }
        free(sentenceCopy1);
    }

    for (i = 0; i < sentenceCount2; i++)
    {
        char *sentenceCopy2 = strdup(sentences2[i]);
        char *word = strtok(sentenceCopy2, " ");
        while (word != NULL)
        {
            size_t wordLength = strlen(word);
            if (wordLength > 0 && ispunct(word[wordLength - 1])) {
                word[wordLength - 1] = '\0';
            }

            int isUnique = 1;
            for (j = 0; j < totalUniqueWordCount; j++)
            {
                if (strcmp(word, uniqueWords[j]) == 0)
                {
                    isUnique = 0;
                    break;
                }
            }

            if (isUnique)
            {
                totalUniqueWordCount++;
                uniqueWords = realloc(uniqueWords, totalUniqueWordCount * sizeof(char *));
                 if (uniqueWords == NULL) {
                    perror("Memory reallocation failed");
                    exit(EXIT_FAILURE);
                }
                uniqueWords[totalUniqueWordCount - 1] = strdup(word);
                 if (uniqueWords[totalUniqueWordCount - 1] == NULL) {
                     perror("Memory allocation failed");
                    exit(EXIT_FAILURE);
                }
            }
            word = strtok(NULL, " ");
        }
         free(sentenceCopy2);
    }

    *uniqueWordCount = totalUniqueWordCount;
    return uniqueWords;
}

int **createOneHotVectors(char *sentences1[], char *sentences2[], int sentenceCount1, int sentenceCount2, char **uniqueWords, int uniqueWordCount)
{
    int totalSentenceCount = sentenceCount1 + sentenceCount2;
    int **oneHotVectors = calloc(totalSentenceCount, sizeof(int *));
    if (oneHotVectors == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    int i, j;

    for (i = 0; i < sentenceCount1; i++)
    {
        oneHotVectors[i] = calloc(uniqueWordCount, sizeof(int));
         if (oneHotVectors[i] == NULL) {
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }

        char *sentenceCopy = strdup(sentences1[i]);
        char *word = strtok(sentenceCopy, " ");
        while (word != NULL)
        {
            size_t wordLength = strlen(word);
            if (wordLength > 0 && ispunct(word[wordLength - 1])) {
                word[wordLength - 1] = '\0';
            }

            int wordIndex = -1;
            for (j = 0; j < uniqueWordCount; j++)
            {
                if (strcmp(word, uniqueWords[j]) == 0)
                {
                    wordIndex = j;
                    break;
                }
            }

            if (wordIndex != -1)
            {
                oneHotVectors[i][wordIndex] = 1;
            }
            word = strtok(NULL, " ");
        }
        free(sentenceCopy);
    }

    for (i = 0; i < sentenceCount2; i++)
    {
        int currentVectorIndex = sentenceCount1 + i;
        oneHotVectors[currentVectorIndex] = calloc(uniqueWordCount, sizeof(int));
         if (oneHotVectors[currentVectorIndex] == NULL) {
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }

        char *sentenceCopy = strdup(sentences2[i]);
        char *word = strtok(sentenceCopy, " ");
        while (word != NULL)
        {
            size_t wordLength = strlen(word);
            if (wordLength > 0 && ispunct(word[wordLength - 1])) {
                word[wordLength - 1] = '\0';
            }

            int wordIndex = -1;
            for (j = 0; j < uniqueWordCount; j++)
            {
                if (strcmp(word, uniqueWords[j]) == 0)
                {
                    wordIndex = j;
                    break;
                }
            }

            if (wordIndex != -1)
            {
                oneHotVectors[currentVectorIndex][wordIndex] = 1;
            }
            word = strtok(NULL, " ");
        }
        free(sentenceCopy);
    }

    return oneHotVectors;
}
