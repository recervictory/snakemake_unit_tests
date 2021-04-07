rule simple_rule3:
    input:
        "input3.txt",
    output:
        "output3.txt",
    shell:
        "echo 'silly goose' > {output}"
