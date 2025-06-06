import pandas as pd
import matplotlib.pyplot as plt
import os

def main():
    # Leer el archivo de estadísticas
    csv_path = 'estadisticas.csv'
    if not os.path.exists(csv_path):
        print(f"No se encontró el archivo: {csv_path}")
        return

    df = pd.read_csv(csv_path)

    # Mostrar las primeras filas (opcional)
    print("Vista previa de las estadísticas:", df.head())

    # Plot: Área vs Slice
    plt.figure(figsize=(10, 6))
    plt.plot(df['Slice'], df['Area'], marker='o', label='Área')
    plt.title('Área del Tumor por Slice')
    plt.xlabel('Slice')
    plt.ylabel('Área')
    plt.grid(True)
    plt.legend()
    plt.savefig("area_vs_slice.png")
    plt.close()

    # Plot: Media vs Slice
    plt.figure(figsize=(10, 6))
    plt.plot(df['Slice'], df['Media'], marker='o', color='orange', label='Media de Intensidad')
    plt.title('Media de Intensidad por Slice')
    plt.xlabel('Slice')
    plt.ylabel('Media de Intensidad')
    plt.grid(True)
    plt.legend()
    plt.savefig("media_vs_slice.png")
    plt.close()

    # Plot: Mínimo y Máximo vs Slice
    plt.figure(figsize=(10, 6))
    plt.plot(df['Slice'], df['Minimo'], marker='o', color='green', label='Valor Mínimo')
    plt.plot(df['Slice'], df['Maximo'], marker='o', color='red', label='Valor Máximo')
    plt.title('Valores Mínimo y Máximo de Intensidad por Slice')
    plt.xlabel('Slice')
    plt.ylabel('Valor de Intensidad')
    plt.grid(True)
    plt.legend()
    plt.savefig("min_max_vs_slice.png")
    plt.close()

    print("Gráficas generadas y guardadas: area_vs_slice.png, media_vs_slice.png, min_max_vs_slice.png")

if __name__ == "__main__":
    main()
