# Лабораторная №1, Вариант 10, условие:
*В файле записаны команды вида: «число<endline>». Дочерний процесс производит
проверку этого числа на простоту. Если число составное, то дочерний процесс пишет это число в
стандартный поток вывода. Если число отрицательное или простое, то тогда дочерний и
родительский процессы завершаются. Количество чисел может быть произвольным*

## Архитектура
*Родительский процесс создает дочерний процесс. Первой строчкой пользователь в консоль
родительского процесса вводит имя файла, которое будет использовано для открытия файла с
таким именем на чтение. Стандартный поток ввода дочернего процесса переопределяется
открытым файлом. Дочерний процесс читает команды из стандартного потока ввода.
Стандартный поток вывода дочернего процесса перенаправляется в pipe1. Родительский процесс
читает из pipe1 и прочитанное выводит в свой стандартный поток вывода. Родительский и
дочерний процесс должны быть представлены разными программами.*

[Отчет](https://docs.google.com/document/d/1Ip3RN82xsAgHl7yowF39v0ivLxKQOYng/edit?usp=sharing&ouid=104732197431944380677&rtpof=true&sd=true)
