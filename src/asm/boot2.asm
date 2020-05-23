section .text
    [global _start64]
    [extern kmain]
    [extern KNL_HIGH_VMA]
    _start64:
        add rsp, KNL_HIGH_VMA

        xor rbp, rbp
        call kmain
