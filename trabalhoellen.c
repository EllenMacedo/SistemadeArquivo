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
void cadastrarUsuario(Usuario* usuarios, int* numUsuarios, char* login, char* senha);
int fazerLogin(Usuario* usuarios, int numUsuarios, char* nome, char* senha);
void alterarUsuario(Usuario* usuarios, int numUsuarios, char* login, char* senha);
void deletarUsuario(Usuario* usuarios, int* numUsuarios, char* login);
void renomearDiretorio(char* nomeUsuario, char* nomeDiretorioAntigo, char* nomeDiretorioNovo);
void moverItem(char* nomeUsuario, char* nomeItem, char* novoDiretorio);
void criarDiretorio(char* nomeUsuario, char* nomeDiretorio);
void modificarUsuario(Usuario* usuarios, int numUsuarios, int indice);
void criarDiretorioUsuario(char* nomeUsuario);
void criarArquivo(char* nomeUsuario, char* nomeArquivo);
void mostrarDiretorioAtual(char* nomeUsuario);
void apagarArquivo(char* nomeUsuario, char* nomeArquivo);
void apagarDiretorioUsuario(char* nomeUsuario);
void renomearArquivo(char* nomeUsuario, char* nomeArquivoAntigo, char* nomeArquivoNovo);
void buscarArquivo(char* nomeUsuario, char* nomeArquivo);

void cadastrarUsuario(Usuario* usuarios, int* numUsuarios, char* login, char* senha) {
    if (*numUsuarios >= MAX_USUARIOS) {
        printf("Numero máximo de usuários atingido.\n");
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
            return i; // Retorna o �ndice do usuario encontrado
        }
    }
    return -1; // Retorna -1 se o login falhar
}

void alterarUsuario(Usuario* usuarios, int numUsuarios, char* login, char* senha) {
    for (int i = 0; i < numUsuarios; i++) {
        if (strcmp(usuarios[i].nome, login) == 0) {
            strcpy(usuarios[i].senha, senha);
            printf("Senha do usuario alterada com sucesso.\n");
            return;
        }
    }
    printf("Usuario nao encontrado.\n");
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
        printf("Usuario nao encontrado.\n");
        return;
    }

    // Remover diretorio do usuario
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
        printf("Erro ao abrir diretorio do usuario.\n");
        return;
    }

    // Remover usuario da lista
    for (int i = indice; i < *numUsuarios - 1; i++) {
        usuarios[i] = usuarios[i + 1];
    }
    (*numUsuarios)--;

    printf("Usuario removido com sucesso.\n");
}

void renomearDiretorio(char* nomeUsuario, char* nomeDiretorioAntigo, char* nomeDiretorioNovo) {
    char caminhoDiretorioAntigo[TAM_CAMINHO];
    char caminhoDiretorioNovo[TAM_CAMINHO];
    sprintf(caminhoDiretorioAntigo, "usuarios/%s/%s", nomeUsuario, nomeDiretorioAntigo);
    sprintf(caminhoDiretorioNovo, "usuarios/%s/%s", nomeUsuario, nomeDiretorioNovo);

    if (rename(caminhoDiretorioAntigo, caminhoDiretorioNovo) == 0) {
        printf("Diretorio renomeado com sucesso.\n");
    } else {
        printf("Erro ao renomear diretorio.\n");
    }
}
void moverItem(char* nomeUsuario, char* nomeItem, char* novoDiretorio) {
    char caminhoItem[TAM_CAMINHO];
    char caminhoDestino[TAM_CAMINHO];
    sprintf(caminhoItem, "usuarios/%s/%s", nomeUsuario, nomeItem);
    sprintf(caminhoDestino, "usuarios/%s/%s", nomeUsuario, novoDiretorio);

    struct stat info;
    if (stat(caminhoDestino, &info) == 0 && S_ISDIR(info.st_mode)) {
        char caminhoDestinoCompleto[TAM_CAMINHO];
        sprintf(caminhoDestinoCompleto, "%s/%s", caminhoDestino, nomeItem);

        if (rename(caminhoItem, caminhoDestinoCompleto) == 0) {
            printf("Item movido com sucesso.\n");
        } else {
            printf("Erro ao mover item.\n");
        }
    } else {
        printf("Destino invalido. O diretorio nao existe.\n");
    }
}
void criarDiretorio(char* nomeUsuario, char* nomeDiretorio) {
    char caminhoDiretorio[TAM_CAMINHO];
    sprintf(caminhoDiretorio, "usuarios/%s/%s", nomeUsuario, nomeDiretorio);

    if (mkdir(caminhoDiretorio) == 0) {
        printf("Diretorio criado com sucesso.\n");
    } else {
        printf("Erro ao criar diretorio.\n");
    }
}

void modificarUsuario(Usuario* usuarios, int numUsuarios, int indice) {
    printf("Modificando usuario: %s\n", usuarios[indice].nome);

    int opcao;
    while (1) {
        printf("Opcoes:\n");
        printf("1. Modificar nome\n");
        printf("2. Modificar senha\n");
        printf("3. Criar diretorio do usuario\n");
        printf("4. Criar arquivo\n");
        printf("5. Mostrar diretorio atual\n");
        printf("6. Apagar arquivo\n");
        printf("7. Apagar diretorio\n");
        printf("8. Renomear arquivo\n");
        printf("9. Renomear diretorio\n");
       printf("10. Mover arquivo ou diretorio\n");
        printf("11. Criar diretorio dentro do diretorio atual\n");
        printf("12. Manipular arquivos de usuarios\n");
        printf("13. Voltar\n");
        printf("Escolha uma opcao: ");
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
            case 5: {
                mostrarDiretorioAtual(usuarios[indice].nome);
                break;
            }
            case 6: {
                char nomeArquivo[TAM_CAMINHO];
                printf("Digite o nome do arquivo para apagar: ");
                scanf("%s", nomeArquivo);
                apagarArquivo(usuarios[indice].nome, nomeArquivo);
                break;
            }
            case 7: {
                apagarDiretorioUsuario(usuarios[indice].nome);
                break;
            }
            case 8: {
                char nomeArquivoAntigo[TAM_CAMINHO];
                char nomeArquivoNovo[TAM_CAMINHO];
                printf("Digite o nome do arquivo a ser renomeado: ");
                scanf("%s", nomeArquivoAntigo);
                printf("Digite o novo nome do arquivo: ");
                scanf("%s", nomeArquivoNovo);
                renomearArquivo(usuarios[indice].nome, nomeArquivoAntigo, nomeArquivoNovo);
                break;
            }
            case 9: {
                char nomeDiretorioAntigo[TAM_CAMINHO];
                char nomeDiretorioNovo[TAM_CAMINHO];
                printf("Digite o nome do diretorio a ser renomeado: ");
                scanf("%s", nomeDiretorioAntigo);
                printf("Digite o novo nome do diretorio: ");
                scanf("%s", nomeDiretorioNovo);
                renomearDiretorio(usuarios[indice].nome, nomeDiretorioAntigo, nomeDiretorioNovo);
                break;
            }
            case 10: {
                char nomeItem[TAM_CAMINHO];
                char novoDiretorio[TAM_CAMINHO];
                printf("Digite o nome do item (arquivo ou diretorio) a ser movido: ");
                scanf("%s", nomeItem);
                printf("Digite o novo diretorio de destino: ");
                scanf("%s", novoDiretorio);
                moverItem(usuarios[indice].nome, nomeItem, novoDiretorio);
                break;
            }
            case 11: {
                char nomeDiretorio[TAM_CAMINHO];
                printf("Digite o nome do diretorio a ser criado: ");
                scanf("%s", nomeDiretorio);
                criarDiretorio(usuarios[indice].nome, nomeDiretorio);
                break;
            }
            case 12: {
                if (usuarios[indice].superusuario) {
                    char nomeUsuario[TAM_NOME_USUARIO];
                    printf("Digite o nome do usuario: ");
                    scanf("%s", nomeUsuario);

                    int indiceUsuario = fazerLogin(usuarios, numUsuarios, nomeUsuario, "");
                    if (indiceUsuario == -1) {
                        printf("Usuario nao encontrado.\n");
                        continue;
                    }

                    modificarUsuario(usuarios, numUsuarios, indiceUsuario);
                } else {
                    printf("Apenas o superusuario pode manipular arquivos de usuarios.\n");
                }
                break;
            }
            case 13: {
                return;
            }
            default:
                printf("Opcao invalida.\n");
                break;
        }
    }
}

void criarDiretorioUsuario(char* nomeUsuario) {
    char nomeDiretorio[TAM_CAMINHO];
    sprintf(nomeDiretorio, "usuarios/%s", nomeUsuario);

    if (mkdir(nomeDiretorio) == 0) {
        printf("Diretorio do usuario criado com sucesso.\n");
    } else {
        printf("Erro ao criar diretorio do usuario.\n");
    }
}

void criarArquivo(char* nomeUsuario, char* nomeArquivo) {
    char caminhoArquivo[TAM_CAMINHO];
    sprintf(caminhoArquivo, "usuarios/%s/%s", nomeUsuario, nomeArquivo);

    FILE* arquivo = fopen(caminhoArquivo, "w");
    if (arquivo) {
        printf("Arquivo criado com sucesso.\n");
        printf("Digite o conteudo do arquivo (Digite '.' em uma nova linha para sair):\n");

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

void mostrarDiretorioAtual(char* nomeUsuario) {
    char nomeDiretorio[TAM_CAMINHO];
    sprintf(nomeDiretorio, "usuarios/%s", nomeUsuario);

    printf("Diretorio atual: %s\n", nomeDiretorio);
}

void apagarArquivo(char* nomeUsuario, char* nomeArquivo) {
    char caminhoArquivo[TAM_CAMINHO];
    sprintf(caminhoArquivo, "usuarios/%s/%s", nomeUsuario, nomeArquivo);

    if (remove(caminhoArquivo) == 0) {
        printf("Arquivo apagado com sucesso.\n");
    } else {
        printf("Erro ao apagar arquivo.\n");
    }
}

void apagarDiretorioUsuario(char* nomeUsuario) {
    char nomeDiretorio[TAM_CAMINHO];
    sprintf(nomeDiretorio, "usuarios/%s", nomeUsuario);

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
        printf("Diretorio do usuario apagado com sucesso.\n");
    } else {
        printf("Erro ao abrir diretorio do usuario.\n");
    }
}

void renomearArquivo(char* nomeUsuario, char* nomeArquivoAntigo, char* nomeArquivoNovo) {
    char caminhoArquivoAntigo[TAM_CAMINHO];
    char caminhoArquivoNovo[TAM_CAMINHO];
    sprintf(caminhoArquivoAntigo, "usuarios/%s/%s", nomeUsuario, nomeArquivoAntigo);
    sprintf(caminhoArquivoNovo, "usuarios/%s/%s", nomeUsuario, nomeArquivoNovo);

    if (rename(caminhoArquivoAntigo, caminhoArquivoNovo) == 0) {
        printf("Arquivo renomeado com sucesso.\n");
    } else {
        printf("Erro ao renomear arquivo.\n");
    }
}

void buscarArquivo(char* nomeUsuario, char* nomeArquivo) {
    char caminhoDiretorio[TAM_CAMINHO];
    sprintf(caminhoDiretorio, "usuarios/%s", nomeUsuario);

    DIR* dir = opendir(caminhoDiretorio);
    if (dir) {
        struct dirent* entrada;
        while ((entrada = readdir(dir)) != NULL) {
            if (entrada->d_type == DT_REG && strcmp(entrada->d_name, nomeArquivo) == 0) {
                printf("Arquivo encontrado: %s\n", entrada->d_name);
                closedir(dir);
                return;
            }
        }
        closedir(dir);
    }

    printf("Arquivo nao encontrado.\n");
}


int main() {
    Usuario usuarios[MAX_USUARIOS];
    int numUsuarios = 0;

    // Criar o superusuario
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
        printf("Digite o nome de usuario: ");
        scanf("%s", nome);

        printf("Digite a senha: ");
        scanf("%s", senha);

        int indice = fazerLogin(usuarios, numUsuarios, nome, senha);

        if (indice == -1) {
            printf("Usuario ou senha invalidos.\n");
            printf("Deseja tentar novamente? (S/N): ");
            char resposta[2];
            scanf("%s", resposta);
            if (strcmp(resposta, "N") == 0 || strcmp(resposta, "n") == 0) {
                break;
            }
        } else {
            printf("Login realizado com sucesso.\n");

            if (usuarios[indice].superusuario) {
                printf("Bem-vindo, superusuario!\n");
                while (1) {
                    char comando[50];
                    printf("Digite um comando (CRIARUSR, ALTERARUSR, DELETARUSR, MODIFICAR, SAIR): ");
                    scanf("%s", comando);

                    if (strcmp(comando, "CRIARUSR") == 0) {
                        if (!usuarios[indice].superusuario) {
                            printf("Apenas o superusuario pode criar novos usuarios.\n");
                            continue;
                        }

                        char novoLogin[TAM_NOME_USUARIO];
                        char novaSenha[TAM_SENHA];

                        printf("Digite o nome do novo usuario: ");
                        scanf("%s", novoLogin);

                        printf("Digite a senha do novo usuario: ");
                        scanf("%s", novaSenha);

                        cadastrarUsuario(usuarios, &numUsuarios, novoLogin, novaSenha);
                    } else if (strcmp(comando, "DELETARUSR") == 0) {
                        if (!usuarios[indice].superusuario) {
                            printf("Apenas o superusuario pode deletar usuarios.\n");
                            continue;
                        }

                        char loginDeletado[TAM_NOME_USUARIO];

                        printf("Digite o nome do usuario a ser deletado: ");
                        scanf("%s", loginDeletado);

                        deletarUsuario(usuarios, &numUsuarios, loginDeletado);
                    } else if (strcmp(comando, "BUSCAR") == 0) {
                        char nomeArquivo[TAM_CAMINHO];
                        printf("Digite o nome do arquivo a ser buscado: ");
                        scanf("%s", nomeArquivo);
                        buscarArquivo(nome, nomeArquivo);
                        }else if (strcmp(comando, "MODIFICAR") == 0) {
                        int indiceUsuario;
                        printf("Digite o nome do usuario a ser modificado: ");
                        scanf("%s", nome);
                        indiceUsuario = fazerLogin(usuarios, numUsuarios, nome, senha);
                        if (indiceUsuario == -1) {
                            printf("Usuario nao encontrado.\n");
                            continue;
                        }
                        modificarUsuario(usuarios, numUsuarios, indiceUsuario);
                    } else if (strcmp(comando, "ALTERARUSR") == 0) {
                        break;
                    } else if (strcmp(comando, "SAIR") == 0) {
                        printf("Encerrando o programa...\n");
                        return 0;
                    } else {
                        printf("Comando invalido.\n");
                    }
                }
            } else {
                printf("Bem-vindo, %s!\n", usuarios[indice].nome);
                modificarUsuario(usuarios, numUsuarios, indice);
            }
        }
    }

    return 0;
}
