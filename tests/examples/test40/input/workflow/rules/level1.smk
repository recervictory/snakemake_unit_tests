rule simple_rule2:
    input:
        "input2.txt",
    output:
        "output2.txt",
    shell:
        "cp {input} {output}"


include: "level2.smk"
