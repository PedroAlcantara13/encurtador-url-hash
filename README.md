# 🔗 Encurtador de URLs com Hash

## 📌 Descrição

Este projeto implementa um encurtador de **URLs** utilizando a linguagem *C*, desenvolvido como parte da disciplina de **Algoritmos e Estruturas de Dados II (AED II)**, no Bacharelado em Tecnologia da Informação - UFERSA  Campus Pau dos Ferros/RN.
O sistema recebe uma URL completa e gera uma chave curta por meio de funções de hash. Com essa chave, é possível recuperar a URL original, promovendo um **mapeamento bidirecional** entre URLs completas e suas versões encurtadas.

## 🧠 Funcionalidades

- Inserção de nova URL para encurtamento  
- Geração automática da chave curta via hash  
- Consulta de URL original a partir da chave curta  

## 🛠 Tecnologias e Conceitos Utilizados

- Linguagem C  
- Estruturas de Dados (Tabelas Hash)  
- Funções de Hash  

## 📂 Estrutura de Arquivos

```
Projeto Final/
│
├── url_encurtador D/
│   ├── .vscode/
│   │    └── tasks.json
│   ├── hash.h
│   |── index.html
│   │── Makefile
|   │── url_shortener
|   │── url_shortener.c
|   │── url_shortener.cgi
|   │── url.db
│

```

## 📝 Exemplo

```bash

# LINK NORMAL:
https://www.google.com/search?q=url+grande+para+teste&sca_esv=44f27cba552c4951&ei=GjmCaJSoJ7va1sQP2saR4AI&ved=0ahUKEwiUndve0NWOAxU7rZUCHVpjBCwQ4dUDCBA&uact=5&oq=url+grande+para+teste&gs_lp=Egxnd3Mtd2l6LXNlcnAiFXVybCBncmFuZGUgcGFyYSB0ZXN0ZTIFECEYoAEyBRAhGJ8FSLMhUIsFWJQhcAN4AZABAJgB2AGgAaURqgEFMC40Lje4AQPIAQD4AQGYAg6gAtoSwgIKEAAYsAMY1gQYR8ICBhAAGBYYHsICCBAAGIAEGKIEwgIEECEYFcICBxAhGKABGAqYAwCIBgGQBgiSBwYzLjEuMTCgB5VCsgcGMC4xLjEwuAewEsIHBjItNC4xMMgHkgE&sclient=gws-wiz-serp

# LINK ENCURTADO:
https://www.google.com/holTha

# CÓDIGO:
holTha

```

## 🚀 Como Executar

```bash
# Pré-requisitos para o acesso 
- Linux ✅
- Servidor Web instalado (Apache com suporte a CGI)✅
- Compilador C (gcc)✅
- Permissão de root ✅

# Processo de Compilação 
 1ª Etapa - Abrir o terminal

    sudo apt update
    sudo apt install apache2 gcc

 2ª Etapa - Ative o módulo CGI

    sudo a2enmod cgi
    sudo systemctl restart apache2

 3ª Etapa - Verifique se o CGI está ativado

    apache2ctl -M | grep cgi

 4ª Etapa - Descompacte o projeto

    unzip url_shortener_project_fixed.zip -d url_shortener
    cd url_shortener

 5ª Etapa - Compile o CGI

    make

 6ª Etapa - Mover os arquivos para o Apache

    sudo cp url_shortener.cgi /usr/lib/cgi-bin/
    sudo touch /usr/lib/cgi-bin/urls.db
    sudo chmod 666 /usr/lib/cgi-bin/urls.db
    sudo chmod +x /usr/lib/cgi-bin/url_shortener.cgi

 7ª Etapa - Interface Web

    sudo cp index.html /var/www/html/

 8ª Etapa - Acessar o sistema

    http://localhost/index.html

## -- 🛠️ Dica de Debug -- ##
Se algo não funcionar, verifique as permições!

    sudo chmod +x /usr/lib/cgi-bin/url_shortener.cgi
    sudo chmod 666 /usr/lib/cgi-bin/urls.db

Ou veja logs do Apache

  sudo tail -f /var/log/apache2/error.log



```

## 👥 Equipe

Ana Jackeline Alves da Silva — Redatora

Maria Vitória Fernandes Rocha — Apresentadora

Marcelo Caat Amaral do Nascimento — Codificador

Pedro Alcântara de Sousa Penaforte— Gitmaster


