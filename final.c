#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>

#define MAX_USUARIOS 10
#define TAM_NOME_USUARIO 20
#define TAM_SENHA 20
#define TAM_CAMINHO 256
#define TAM_NOME_ARQUIVO 15
#define TAM_EXTENSAO 4

typedef struct {
    char nome[TAM_NOME_USUARIO];
    char senha[TAM_SENHA];
    int superusuario;
} Usuario;

typedef struct {
    int ID;
    char nome[TAM_NOME_ARQUIVO];
    char extensao[TAM_EXTENSAO];
    char proprietario[TAM_NOME_USUARIO];
    time_t dataCriacao;
} Arquivo;

void cadastrarUsuario(Usuario* usuarios, int* numUsuarios, char* login, char* senha) {
    if (*numUsuarios >= MAX_USUARIOS) {
        printf("Número máximo de usuários atingido.\n");
        return;
    }

    Usuario novoUsuario;
    strcpy(novoUsuario.nome, login);
    strcpy(novoUsuario.senha, senha);
    novoUsuario.superusuario = 0;

    // Criar diretório "usuarios/" se ainda não existir
    if (mkdir("usuarios") != 0 && errno != EEXIST) {
        printf("Erro ao criar diretório para os usuários.\n");
        return;
    }

    // Criar diretório para o novo usuário
    char nomeDiretorio[TAM_NOME_USUARIO + 10]; // +10 para acomodar o prefixo do caminho
    sprintf(nomeDiretorio, "usuarios/%s", login);

    if (mkdir(nomeDiretorio) == 0) {
        printf("Usuário criado com sucesso.\n");
        usuarios[*numUsuarios] = novoUsuario;
        (*numUsuarios)++;
    } else {
        printf("Erro ao criar diretório para o usuário.\n");
    }
}

int fazerLogin(Usuario* usuarios, int numUsuarios, char* nome, char* senha) {
    for (int i = 0; i < numUsuarios; i++) {
        if (strcmp(usuarios[i].nome, nome) == 0 && strcmp(usuarios[i].senha, senha) == 0) {
            return i; // Retorna o índice do usuário encontrado
        }
    }
    return -1; // Retorna -1 se o login falhar
}

void alterarUsuario(Usuario* usuarios, int numUsuarios, char* login, char* senha) {
    for (int i = 0; i < numUsuarios; i++) {
        if (strcmp(usuarios[i].nome, login) == 0) {
            strcpy(usuarios[i].senha, senha);
            printf("Senha do usuário alterada com sucesso.\n");
            return;
        }
    }
    printf("Usuário não encontrado.\n");
}

void deletarUsuario(Usuario* usuarios, int* numUsuarios, char* login) {
    int indice = -1;
    for (int i = 0; i < *numUsuarios; i++) {
        if (strcmp(usuarios[i].nome, login) == 0) {
            indice = i;
            break;
        }
    }

    if (indice == -1) {
        printf("Usuário não encontrado.\n");
        return;
    }

    // Remover diretório do usuário
    char nomeDiretorio[TAM_NOME_USUARIO + 10]; // +10 para acomodar o prefixo do caminho
    sprintf(nomeDiretorio, "usuarios/%s", login);

    DIR* dir = opendir(nomeDiretorio);
    if (dir) {
        struct dirent* entrada;
        while ((entrada = readdir(dir)) != NULL) {
            if (strcmp(entrada->d_name, ".") != 0 && strcmp(entrada->d_name, "..") != 0) {
                char caminhoArquivo[TAM_CAMINHO];
                sprintf(caminhoArquivo, "%s/%s", nomeDiretorio, entrada->d_name);
                remove(caminhoArquivo);
            }
        }
        closedir(dir);
        rmdir(nomeDiretorio);
    } else {
        printf("Erro ao abrir diretório do usuário.\n");
        return;
    }

    // Remover usuário da lista
    for (int i = indice; i < *numUsuarios - 1; i++) {
        usuarios[i] = usuarios[i + 1];
    }
    (*numUsuarios)--;

    printf("Usuário removido com sucesso.\n");
}

void modificarUsuario(Usuario* usuarios, int numUsuarios, int indice) {
    printf("Modificando usuário: %s\n", usuarios[indice].nome);

    int opcao;
    printf("Opções:\n");
    printf("1. Modificar nome\n");
    printf("2. Modificar senha\n");
    printf("3. Voltar\n");
    printf("Escolha uma opção: ");
    scanf("%d", &opcao);

    switch (opcao) {
        case 1: {
            char novoNome[TAM_NOME_USUARIO];
            printf("Digite o novo nome: ");
            scanf("%s", novoNome);
            strcpy(usuarios[indice].nome, novoNome);
            printf("Nome modificado com sucesso.\n");
            break;
        }
        case 2: {
            char novaSenha[TAM_SENHA];
            printf("Digite a nova senha: ");
            scanf("%s", novaSenha);
            strcpy(usuarios[indice].senha, novaSenha);
            printf("Senha modificada com sucesso.\n");
            break;
        }
        case 3:
            printf("Voltando...\n");
            break;
        default:
            printf("Opção inválida.\n");
            break;
    }
}

void criarArquivo(Usuario* usuarios, int indice) {
    if (!usuarios[indice].superusuario) {
        printf("Apenas o superusuário pode criar novos arquivos.\n");
        return;
    }

    Arquivo novoArquivo;
    int novoID;

    printf("Digite o ID do arquivo: ");
    scanf("%d", &novoID);

    if (novoID <= 0) {
        printf("O ID do arquivo deve ser um número positivo.\n");
        return;
    }

    for (int i = 0; i < novoID; i++) {
        char nomeArquivo[TAM_NOME_ARQUIVO + TAM_EXTENSAO + 2]; // +2 para acomodar o ponto (.) e o caractere nulo (\0)
        sprintf(nomeArquivo, "%s.%s", usuarios[indice].nome, usuarios[indice].senha);
        if (strlen(nomeArquivo) > TAM_NOME_ARQUIVO + TAM_EXTENSAO + 1) {
            printf("O nome do arquivo é muito longo. Não pode exceder %d caracteres.\n", TAM_NOME_ARQUIVO + TAM_EXTENSAO + 1);
            return;
        }
        if (i > 0) {
            nomeArquivo[strlen(nomeArquivo) - 1] = '\0'; // Remove a extensão anterior para criar o novo nome
        }
        strcpy(novoArquivo.nome, nomeArquivo);
        strcpy(novoArquivo.extensao, "txt");
        strcpy(novoArquivo.proprietario, usuarios[indice].nome);
        novoArquivo.dataCriacao = time(NULL);
        novoArquivo.ID = i + 1;

        char nomeDiretorio[TAM_NOME_USUARIO + 10]; // +10 para acomodar o prefixo do caminho
        sprintf(nomeDiretorio, "usuarios/%s", usuarios[indice].nome);

        char caminhoArquivo[TAM_CAMINHO];
        sprintf(caminhoArquivo, "%s/%s.%s", nomeDiretorio, novoArquivo.nome, novoArquivo.extensao);

        FILE* arquivo = fopen(caminhoArquivo, "w");
        if (arquivo) {
            fwrite(&novoArquivo, sizeof(Arquivo), 1, arquivo);
            fclose(arquivo);
            printf("Arquivo criado com sucesso.\n");
        } else {
            printf("Erro ao criar o arquivo.\n");
            return;
        }
    }
}

void listarAtributos(Usuario* usuarios, int indice) {
    if (!usuarios[indice].superusuario) {
        printf("Apenas o superusuário pode listar os atributos dos arquivos.\n");
        return;
    }

    char nomeDiretorio[TAM_NOME_USUARIO + 10]; // +10 para acomodar o prefixo do caminho
    sprintf(nomeDiretorio, "usuarios/%s", usuarios[indice].nome);

    DIR* dir = opendir(nomeDiretorio);
    if (dir) {
        struct dirent* entrada;
        while ((entrada = readdir(dir)) != NULL) {
            if (strcmp(entrada->d_name, ".") != 0 && strcmp(entrada->d_name, "..") != 0) {
                char caminhoArquivo[TAM_CAMINHO];
                sprintf(caminhoArquivo, "%s/%s", nomeDiretorio, entrada->d_name);

                FILE* arquivo = fopen(caminhoArquivo, "r");
                if (arquivo) {
                    Arquivo atributos;
                    fread(&atributos, sizeof(Arquivo), 1, arquivo);
                    fclose(arquivo);

                    struct tm* tm_info;
                    char buffer[20];
                    tm_info = localtime(&atributos.dataCriacao);
                    strftime(buffer, 20, "%Y-%m-%d %H:%M:%S", tm_info);

                    printf("ID: %d\n", atributos.ID);
                    printf("Nome: %s\n", atributos.nome);
                    printf("Extensão: %s\n", atributos.extensao);
                    printf("Proprietário: %s\n", atributos.proprietario);
                    printf("Data de criação: %s\n", buffer);
                    printf("\n");
                }
            }
        }
        closedir(dir);
    } else {
        printf("Erro ao abrir diretório do usuário.\n");
        return;
    }
}

int validarNome(char* nome) {
    if (strlen(nome) == 0 || strlen(nome) > TAM_NOME_ARQUIVO) {
        return 0;
    }

    for (int i = 0; i < strlen(nome); i++) {
        if (!isalpha(nome[i]) || isspace(nome[i]) || ispunct(nome[i])) {
            return 0;
        }
    }

    return 1;
}

void removerDiretoriosVazios(Usuario* usuarios, int numUsuarios) {
    for (int i = 0; i < numUsuarios; i++) {
        char nomeDiretorio[TAM_NOME_USUARIO + 10]; // +10 para acomodar o prefixo do caminho
        sprintf(nomeDiretorio, "usuarios/%s", usuarios[i].nome);

        DIR* dir = opendir(nomeDiretorio);
        if (dir) {
            struct dirent* entrada;
            int arquivosEncontrados = 0;
            while ((entrada = readdir(dir)) != NULL) {
                if (strcmp(entrada->d_name, ".") != 0 && strcmp(entrada->d_name, "..") != 0) {
                    arquivosEncontrados = 1;
                    break;
                }
            }
            closedir(dir);

            if (!arquivosEncontrados) {
                rmdir(nomeDiretorio);
            }
        }
    }
}

int main() {
    Usuario usuarios[MAX_USUARIOS];
    int numUsuarios = 0;

    // Criar o superusuário
    Usuario superusuario;
    strcpy(superusuario.nome, "admin");
    strcpy(superusuario.senha, "admin123");
    superusuario.superusuario = 1;
    usuarios[numUsuarios] = superusuario;
    numUsuarios++;

    char nome[TAM_NOME_USUARIO];
    char senha[TAM_SENHA];

    printf("Login\n");
    printf("Digite o nome de usuário: ");
    scanf("%s", nome);

    printf("Digite a senha: ");
    scanf("%s", senha);

    int indice = fazerLogin(usuarios, numUsuarios, nome, senha);

    if (indice == -1) {
        printf("Usuário ou senha inválidos. Encerrando o programa.\n");
        return 0;
    }

    printf("Login realizado com sucesso.\n");

    if (usuarios[indice].superusuario) {
        printf("Bem-vindo, superusuário!\n");
        int opcao;
        printf("Opções:\n");
        printf("1. Cadastrar novo usuário\n");
        printf("2. Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1: {
                char novoLogin[TAM_NOME_USUARIO];
                char novaSenha[TAM_SENHA];
                printf("Digite o nome de usuário: ");
                scanf("%s", novoLogin);
                printf("Digite a senha: ");
                scanf("%s", novaSenha);
                cadastrarUsuario(usuarios, &numUsuarios, novoLogin, novaSenha);
                break;
            }
            case 2:
                printf("Encerrando o programa.\n");
                removerDiretoriosVazios(usuarios, numUsuarios);
                return 0;
            default:
                printf("Opção inválida. Encerrando o programa.\n");
                removerDiretoriosVazios(usuarios, numUsuarios);
                return 0;
        }
    } else {
        printf("Bem-vindo, %s!\n", usuarios[indice].nome);
        modificarUsuario(usuarios, numUsuarios, indice);
    }

    // Interpretador de comandos
    char comando[12];

    while (1) {
        printf("\nDigite um comando: ");
        scanf("%s", comando);

        if (strcmp(comando, "CRIARUSR") == 0) {
            if (!usuarios[indice].superusuario) {
                printf("Apenas o superusuário pode criar novos usuários.\n");
                continue;
            }

            char novoLogin[TAM_NOME_USUARIO];
            char novaSenha[TAM_SENHA];
            printf("Digite o nome de usuário: ");
            scanf("%s", novoLogin);
            printf("Digite a senha: ");
            scanf("%s", novaSenha);
            cadastrarUsuario(usuarios, &numUsuarios, novoLogin, novaSenha);
        } else if (strcmp(comando, "ALTERARUSR") == 0) {
            char novoLogin[TAM_NOME_USUARIO];
            char novaSenha[TAM_SENHA];
            printf("Digite o nome de usuário: ");
            scanf("%s", novoLogin);
            printf("Digite a nova senha: ");
            scanf("%s", novaSenha);
            alterarUsuario(usuarios, numUsuarios, novoLogin, novaSenha);
        } else if (strcmp(comando, "DELETARUSR") == 0) {
            if (!usuarios[indice].superusuario) {
                printf("Apenas o superusuário pode deletar usuários.\n");
                continue;
            }

            char loginDeletar[TAM_NOME_USUARIO];
            printf("Digite o nome de usuário a ser deletado: ");
            scanf("%s", loginDeletar);
            deletarUsuario(usuarios, &numUsuarios, loginDeletar);
        } else if (strcmp(comando, "CRIARARQ") == 0) {
            criarArquivo(usuarios, indice);
        } else if (strcmp(comando, "LISTARATR") == 0) {
            listarAtributos(usuarios, indice);
        } else if (strcmp(comando, "SAIR") == 0) {
            printf("Logout realizado com sucesso.\n");
            break;
        } else {
            printf("Comando inválido.\n");
        }
    }

    removerDiretoriosVazios(usuarios, numUsuarios);
    return 0;
}
