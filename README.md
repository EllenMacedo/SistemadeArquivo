# SistemadeArquivo
sistema de arquivos e interpretador de comandos em C
O que e "MAIUSCULO/CAIXA ALTA/CAPS LOCK " tem que ser escrito assim os comandos.

SuperUsuario
Login: 
admin
Senha: 
admin123

EXEMPLO:
1) admin>admin123>CRIARUSR>fulano>senha123>NOVOLOGIN>fulano>senha123>MODIFICAR>3>4>nomearquivo>conteudo>.>11>subpasta>10>nomearquivo>subpasta>12>SAIR.
2) fulano>123>N (N=desistiu de continuar o login, o programa encerra).


CRIARUSR - cria um novo usuário.
ALTERARUSR - fará o login do novo usuário
DELETARUSR  - Apagará o usuário.
SAIR - deverá fazer logout do sistema. 
MODIFICAR - manipulacao dos arquivos, opçoes: 
 printf("Opções:\n");
       1. Modificar nome
       2. Modificar senha
       3. Criar diretório do usuário
       4. Criar arquivo
       5. Mostrar diretório atual
       6. Apagar arquivo
       7. Apagar diretório
       8. Renomear arquivo
       9. Renomear diretório
       10. Mover arquivo ou diretório
       11. Criar diretório dentro do diretório atual
       12. Voltar
       13. Manipular arquivos de usuários

  
