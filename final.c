#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>

#define MAX_USUARIOS 10
#define TAM_NOME_USUARIO 20
#define TAM_SENHA 20
#define TAM_CAMINHO 256

typedef struct {
    char nome[TAM_NOME_USUARIO];
    char senha[TAM_SENHA];
    int superusuario;
} Usuario;

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
    while (1) {
        printf("Opções:\n");
        printf("1. Modificar nome\n");
        printf("2. Modificar senha\n");
        printf("3. Criar diretório do usuário\n");
        printf("4. Criar arquivo\n");
        printf("5. Voltar\n");
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
            case 3: {
                criarDiretorioUsuario(usuarios[indice].nome);
                break;
            }
            case 4: {
                char nomeArquivo[TAM_CAMINHO];
                printf("Digite o nome do arquivo: ");
                scanf("%s", nomeArquivo);
                criarArquivo(usuarios[indice].nome, nomeArquivo);
                break;
            }
            case 5:
                printf("Voltando...\n");
                return;
            default:
                printf("Opção inválida.\n");
                break;
        }
    }
}

void criarDiretorioUsuario(char* nomeUsuario) {
    char nomeDiretorio[TAM_CAMINHO];
    sprintf(nomeDiretorio, "usuarios/%s", nomeUsuario);

    if (mkdir(nomeDiretorio) == 0) {
        printf("Diretório do usuário criado com sucesso.\n");
    } else {
        printf("Erro ao criar diretório do usuário.\n");
    }
}

void criarArquivo(char* nomeUsuario, char* nomeArquivo) {
    char caminhoArquivo[TAM_CAMINHO];
    sprintf(caminhoArquivo, "usuarios/%s/%s", nomeUsuario, nomeArquivo);

    FILE* arquivo = fopen(caminhoArquivo, "w");
    if (arquivo) {
        printf("Arquivo criado com sucesso.\n");
        printf("Digite o conteúdo do arquivo (Digite '.' em uma nova linha para sair):\n");

        char linha[100];
        while (1) {
            fgets(linha, sizeof(linha), stdin);
            if (strcmp(linha, ".\n") == 0) {
                break;
            }
            fprintf(arquivo, "%s", linha);
        }

        fclose(arquivo);
    } else {
        printf("Erro ao criar arquivo.\n");
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

    while (1) {
        printf("Login\n");
        printf("Digite o nome de usuário: ");
        scanf("%s", nome);

        printf("Digite a senha: ");
        scanf("%s", senha);

        int indice = fazerLogin(usuarios, numUsuarios, nome, senha);

        if (indice == -1) {
            printf("Usuário ou senha inválidos.\n");
            printf("Deseja tentar novamente? (S/N): ");
            char resposta[2];
            scanf("%s", resposta);
            if (strcmp(resposta, "N") == 0 || strcmp(resposta, "n") == 0) {
                break;
            }
        } else {
            printf("Login realizado com sucesso.\n");

            if (usuarios[indice].superusuario) {
                printf("Bem-vindo, superusuário!\n");
                while (1) {
                    char comando[50];
                    printf("Digite um comando (CRIARUSR, ALTERARUSR, DELETARUSR, MODIFICAR, NOVOLOGIN, SAIR): ");
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
                    } else if (strcmp(comando, "MODIFICAR") == 0) {
                        modificarUsuario(usuarios, numUsuarios, indice);
                    } else if (strcmp(comando, "NOVOLOGIN") == 0) {
                        break;
                    } else if (strcmp(comando, "SAIR") == 0) {
                        printf("Logout realizado com sucesso.\n");
                        return 0;
                    } else {
                        printf("Comando inválido.\n");
                    }
                }
            } else {
                printf("Bem-vindo, %s!\n", usuarios[indice].nome);
                while (1) {
                    char comando[50];
                    printf("Digite um comando (MODIFICAR, NOVOLOGIN, SAIR): ");
                    scanf("%s", comando);

                    if (strcmp(comando, "MODIFICAR") == 0) {
                        modificarUsuario(usuarios, numUsuarios, indice);
                    } else if (strcmp(comando, "NOVOLOGIN") == 0) {
                        break;
                    } else if (strcmp(comando, "SAIR") == 0) {
                        printf("Logout realizado com sucesso.\n");
                        return 0;
                    } else {
                        printf("Comando inválido.\n");
                    }
                }
            }
        }
    }

    return 0;
}
