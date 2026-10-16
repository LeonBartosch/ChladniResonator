import numpy as np
import matplotlib.pyplot as plt
from scipy.linalg import eigh
from scipy.ndimage import gaussian_filter
from numpy import trapezoid


# ============================================================
# CONFIG
# ============================================================
Nx_mode = 10
Ny_mode = 8
Nplot_x = 300
Nplot_y = 150

Lx = 2.0
Ly = 0.4


# ============================================================
# FREE–FREE 1D Euler-Bernoulli Mode
# ============================================================
def free_free_mode(n, x, L):
    k = n * np.pi / L
    alpha = (np.cos(k*L) + np.cosh(k*L)) / (np.sin(k*L) + np.sinh(k*L))
    w = np.cos(k*x) + np.cosh(k*x) - alpha*(np.sin(k*x) + np.sinh(k*x))
    w /= np.sqrt(trapezoid(w**2, x))
    return w


def build_modes(Nm, L, Ngrid):
    x = np.linspace(0, L, Ngrid)
    modes = [free_free_mode(n, x, L) for n in range(1, Nm+1)]
    return x, np.array(modes)


x_grid, phi_x = build_modes(Nx_mode, Lx, Nplot_x)
y_grid, phi_y = build_modes(Ny_mode, Ly, Nplot_y)


# Derivatives
phi_x2 = np.gradient(np.gradient(phi_x, x_grid, axis=1), x_grid, axis=1)
phi_y2 = np.gradient(np.gradient(phi_y, y_grid, axis=1), y_grid, axis=1)


# ============================================================
# MATERIAL
# ============================================================
XX,YY = np.meshgrid(x_grid/Lx, y_grid/Ly)

thick = 1.0 + 0.3*(0.5 - YY)

Dx  = 3e9 * thick**3
Dy  = 1.5e9 * thick**3
Dxy = 0.4e9 * thick**3

Dx  = gaussian_filter(Dx, 3)
Dy  = gaussian_filter(Dy, 3)
Dxy = gaussian_filter(Dxy, 3)


# ============================================================
# BUILD 2D GALERKIN MATRICES
# ============================================================
modes = [(i, j) for i in range(Nx_mode) for j in range(Ny_mode)]
K = len(modes)

A = np.zeros((K, K))
B = np.zeros((K, K))

for I, (ix, iy) in enumerate(modes):

    px  = phi_x[ix]
    py  = phi_y[iy]
    px2 = phi_x2[ix]
    py2 = phi_y2[iy]

    psi = np.outer(py, px)
    psi_xx = np.outer(py, px2)
    psi_yy = np.outer(py2, px)
    psi_xy = np.gradient(np.gradient(psi, x_grid, axis=1), y_grid, axis=0)

    for J, (jx, jy) in enumerate(modes):

        qx  = phi_x[jx]
        qy  = phi_y[jy]
        qx2 = phi_x2[jx]
        qy2 = phi_y2[jy]

        phi2 = np.outer(qy, qx)
        phi_xx = np.outer(qy, qx2)
        phi_yy = np.outer(qy2, qx)
        phi_xy = np.gradient(np.gradient(phi2, x_grid, axis=1), y_grid, axis=0)

        # MASS (positive definite except nullspace)
        B[I,J] = trapezoid(trapezoid(psi * phi2, x_grid), y_grid)

        # STIFFNESS
        integrand = (
            Dx * psi_xx * phi_xx +
            Dy * psi_yy * phi_yy +
            2 * Dxy * psi_xy * phi_xy
        )
        A[I,J] = trapezoid(trapezoid(integrand, x_grid), y_grid)


# ============================================================
# NULLMODEN ENTFERNEN (Free–Free)
# ============================================================
# Nullmode 1: konstante Verschiebung
# Nullmode 2: Rotation in x
# Nullmode 3: Rotation in y
null_idx = [0, 1, 2]

mask = np.ones(K, dtype=bool)
mask[null_idx] = False

A2 = A[mask][:, mask]
B2 = B[mask][:, mask]


# ============================================================
# SOLVE
# ============================================================
vals, vecs = eigh(A2, B2)

order = np.argsort(vals)
vals = vals[order]
vecs = vecs[:, order]


# ============================================================
# MODE EVALUATION
# ============================================================
def eval_mode(k):
    c = vecs[:, k]
    field = np.zeros_like(XX)

    idx = 0
    for ii,(i,j) in enumerate(modes):
        if not mask[ii]: 
            continue
        field += c[idx] * np.outer(phi_y[j], phi_x[i])
        idx += 1
    return field


# ============================================================
# PLOT
# ============================================================
fig, axs = plt.subplots(3, 4, figsize=(14, 8))
Nshow = 12

for k, ax in enumerate(axs.flat):
    f = eval_mode(k)
    ax.imshow(f, cmap="RdBu", origin="lower", extent=[0, Lx, 0, Ly])
    ax.set_title(f"Mode {k+1}\nλ={vals[k]:.2e}", fontsize=8)
    ax.axis("off")

plt.tight_layout()
plt.show()