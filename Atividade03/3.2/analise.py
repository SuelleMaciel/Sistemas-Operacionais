import sys
import os
import pandas as pd
import matplotlib.pyplot as plt

if len(sys.argv) < 2:
    sys.exit("Uso: python3 analise.py tempos_A.csv [tempos_B.csv ...]")

resumos = {}

for arquivo in sys.argv[1:]:
    nome = os.path.basename(arquivo).replace("tempos_", "").replace(".csv", "")
    df = pd.read_csv(arquivo)

    brutos = df.pivot(index="threads", columns="execucao", values="tempo_real")
    brutos.columns = [f"exec {c}" for c in brutos.columns]

    g = df.groupby("threads")["tempo_real"]
    resumo = pd.DataFrame({
        "media (s)": g.mean(),
        "desvio (s)": g.std(ddof=1),
    })
    resumo["CV (%)"] = resumo["desvio (s)"] / resumo["media (s)"] * 100
    resumo["normalizado"] = resumo["media (s)"] / resumo.loc[1, "media (s)"]
    resumo["speedup"] = resumo.loc[1, "media (s)"] / resumo["media (s)"]
    resumos[nome] = resumo

    tabela = brutos.join(resumo).round(3)
    tabela.to_csv(f"tabela_{nome}.csv")
    print(f"\n===== {nome} =====")
    print(tabela.to_string())

    ruins = resumo[resumo["CV (%)"] > 5]
    if not ruins.empty:
        print(f"ATENÇÃO: CV acima de 5% para threads = {list(ruins.index)} -> refazer medições")

    plt.figure(figsize=(7, 4.5))
    plt.errorbar(resumo.index, resumo["media (s)"], yerr=resumo["desvio (s)"],
                 marker="o", capsize=4)
    plt.xscale("log", base=2)
    plt.xticks(resumo.index, resumo.index)
    plt.xlabel("Número de threads")
    plt.ylabel("Tempo médio de execução (s)")
    plt.title(f"Tempo médio de execução — {nome}")
    plt.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.savefig(f"grafico_{nome}.png", dpi=150)
    plt.close()

plt.figure(figsize=(7, 4.5))
for nome, resumo in resumos.items():
    plt.plot(resumo.index, resumo["normalizado"], marker="o", label=nome)
plt.xscale("log", base=2)
plt.xticks([1, 2, 4, 8, 16, 32], [1, 2, 4, 8, 16, 32])
plt.xlabel("Número de threads")
plt.ylabel("Tempo normalizado (T(N) / T(1))")
plt.title("Tempos normalizados — todas as plataformas")
plt.ylim(bottom=0)
plt.grid(True, alpha=0.3)
plt.legend()
plt.tight_layout()
plt.savefig("grafico_consolidado.png", dpi=150)
plt.close()

print("\nGráficos salvos: grafico_<plataforma>.png e grafico_consolidado.png")
