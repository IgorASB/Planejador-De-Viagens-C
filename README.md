# Planejador de Viagens em C

Planejador de viagens em C com cadastro de viagens, matriz dinâmica de gastos por dia e salvamento em arquivo texto.

## Sobre o projeto

Este projeto é um planejador de viagens em linha de comando. Você pode cadastrar viagens com nome, destino e quantidade de dias, registrar gastos diários por categoria (hospedagem, transporte e alimentação) e visualizar a matriz completa de gastos por dia, além do total por viagem.

### Conceitos de C praticados

- Structs para representar viagens e o gerenciador de viagens
- Vetores dinâmicos de structs com `malloc` e `realloc`
- Matrizes dinâmicas de `float` (`float **`) para armazenar gastos `dias × categorias`
- Ponteiros em funções para manipular o gerenciador
- Funções para modularizar a lógica (cadastro, listagem, registro de gastos, persistência)
- Manipulação de arquivos texto para salvar e carregar os dados (`viagens.txt`)
- Laços de repetição, validação de entrada e menus interativos

## Como compilar e executar

### Pré-requisitos

- Compilador C (gcc, clang ou equivalente)

### Compilação

```bash
gcc planejar_viagens.c -o planejar_viagens
```

### Execução

```bash
./planejar_viagens      # Linux/macOS
planejar_viagens.exe    # Windows
```

O programa cria e utiliza o arquivo `viagens.txt` na mesma pasta para armazenar os dados das viagens e da matriz de gastos.

## Funcionalidades

- Cadastrar novas viagens (nome, destino, quantidade de dias)
- Registrar gastos por dia e por categoria
- Listar viagens com total de gastos
- Mostrar a matriz de gastos de uma viagem (dias × categorias) com total por dia e total geral
- Salvar automaticamente os dados ao sair e recarregar na próxima execução

## Estrutura principal do código

- `GerenciadorViagens`: struct que guarda o vetor dinâmico de viagens
- `Viagem`: struct com nome, destino, quantidade de dias e ponteiro para a matriz de gastos
- Funções de inicialização, cadastro, listagem, registro de gastos, exibição da matriz e persistência em arquivo

## Licença

Este projeto é de uso livre para estudo e prática de linguagem C. Sinta-se à vontade para adaptar e evoluir o código.
