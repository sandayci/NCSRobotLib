/**
 * Constraints

 l0: unitary Rx
 l1: unitary Ry
 l2: unitary Rz

 l3: ortho Rx Ry
 l4: ortho Rx Rz
 l5: ortho Ry Rz
 
**/

// Rxr
   2*QRxrRxr*Rxr + QRxrRxs*Rxs + QRxrRxt*Rxt + QRxrRyr*Ryr + QRxrRys*Rys + QRxrRyt*Ryt + QRxrRzr*Rzr + QRxrRzs*Rzs + QRxrRzt*Rzt + QRxrTx*Tx + QRxrTy*Ty + QRxrTz*Tz
 + QRxr
 + l0 * (2*Rxr0 - 1.)
 + l3 * Ryr0
 + l4 * Rzr0
 
// Rxs
   QRxrRxs*Rxr + 2*QRxsRxs*Rxs + QRxsRxt*Rxt + QRxsRyr*Ryr + QRxsRys*Rys + QRxsRyt*Ryt + QRxsRzr*Rzr + QRxsRzs*Rzs + QRxsRzt*Rzt + QRxsTx*Tx + QRxsTy*Ty + QRxsTz*Tz
 + QRxs
 + l0 * (2*Rxs0 - 1.)
 + l3 * Rys0
 + l4 * Rzs0
 
// Rxt
   QRxrRxt*Rxr + QRxsRxt*Rxs + 2*QRxtRxt*Rxt + QRxtRyr*Ryr + QRxtRys*Rys + QRxtRyt*Ryt + QRxtRzr*Rzr + QRxtRzs*Rzs + QRxtRzt*Rzt + QRxtTx*Tx + QRxtTy*Ty + QRxtTz*Tz
   + QRxt
 + l0 * (2*Rxt0 - 1.)
 + l3 * Ryt0
 + l4 * Rzt0 
 
// Ryr
   QRxrRyr*Rxr + QRxsRyr*Rxs + QRxtRyr*Rxt + 2*QRyrRyr*Ryr + QRyrRys*Rys + QRyrRyt*Ryt + QRyrRzr*Rzr + QRyrRzs*Rzs + QRyrRzt*Rzt + QRyrTx*Tx + QRyrTy*Ty + QRyrTz*Tz
 + QRyr
 + l1 * (2*Ryr0 - 1.)
 + l3 * Rxr0
 + l5 * Rzr0 
 
// Rys
   QRxrRys*Rxr + QRxsRys*Rxs + QRxtRys*Rxt + QRyrRys*Ryr + 2*QRysRys*Rys + QRysRyt*Ryt + QRysRzr*Rzr + QRysRzs*Rzs + QRysRzt*Rzt + QRysTx*Tx + QRysTy*Ty + QRysTz*Tz
 + QRys
 + l1 * (2*Rys0 - 1.)
 + l3 * Rxs0
 + l5 * Rzs0 
 
// Ryt
   QRxrRyt*Rxr + QRxsRyt*Rxs + QRxtRyt*Rxt + QRyrRyt*Ryr + QRysRyt*Rys + 2*QRytRyt*Ryt + QRytRzr*Rzr + QRytRzs*Rzs + QRytRzt*Rzt + QRytTx*Tx + QRytTy*Ty + QRytTz*Tz
 + QRyt
 + l1 * (2*Ryt0 - 1.)
 + l3 * Rxt0
 + l5 * Rzt0 
 
// Rzr
   QRxrRzr*Rxr + QRxsRzr*Rxs + QRxtRzr*Rxt + QRyrRzr*Ryr + QRysRzr*Rys + QRytRzr*Ryt + 2*QRzrRzr*Rzr + QRzrRzs*Rzs + QRzrRzt*Rzt + QRzrTx*Tx + QRzrTy*Ty + QRzrTz*Tz
 + QRzr
 + l2 * (2*Rzr0 - 1.)
 + l4 * Rxr0
 + l5 * Ryr0 
 
// Rzs
   QRxrRzs*Rxr + QRxsRzs*Rxs + QRxtRzs*Rxt + QRyrRzs*Ryr + QRysRzs*Rys + QRytRzs*Ryt + QRzrRzs*Rzr + 2*QRzsRzs*Rzs + QRzsRzt*Rzt + QRzsTx*Tx + QRzsTy*Ty + QRzsTz*Tz
 + QRzs
 + l2 * (2*Rzs0 - 1.)
 + l4 * Rxs0
 + l5 * Rys0 
 
// Rzt
   QRxrRzt*Rxr + QRxsRzt*Rxs + QRxtRzt*Rxt + QRyrRzt*Ryr + QRysRzt*Rys + QRytRzt*Ryt + QRzrRzt*Rzr + QRzsRzt*Rzs + 2*QRztRzt*Rzt + QRztTx*Tx + QRztTy*Ty + QRztTz*Tz
 + QRzt
 + l2 * (2*Rzt0 - 1.)
 + l4 * Rxt0
 + l5 * Ryt0  
 
// Tx
   QRxrTx*Rxr + QRxsTx*Rxs + QRxtTx*Rxt + QRyrTx*Ryr + QRysTx*Rys + QRytTx*Ryt + QRzrTx*Rzr + QRzsTx*Rzs + QRztTx*Rzt + 2*QTxTx*Tx + QTxTy*Ty + QTxTz*Tz
 + QTx
 
// Ty
   QRxrTy*Rxr + QRxsTy*Rxs + QRxtTy*Rxt + QRyrTy*Ryr + QRysTy*Rys + QRytTy*Ryt + QRzrTy*Rzr + QRzsTy*Rzs + QRztTy*Rzt + QTxTy*Tx + 2*QTyTy*Ty + QTyTz*Tz
 + QTy
 
// Tz
   QRxrTz*Rxr + QRxsTz*Rxs + QRxtTz*Rxt + QRyrTz*Ryr + QRysTz*Rys + QRytTz*Ryt + QRzrTz*Rzr + QRzsTz*Rzs + QRztTz*Rzt + QTxTz*Tx + QTyTz*Ty + 2*QTzTz*Tz
 + QTz
 
// l0 (linearized)
   2*Rxr0*Rxr + 2*Rxs0*Rxs + 2*Rxt0*Rxt 
 + Rxr0 + Rxs0 + Rxt0 
 - 2*Rxr0*Rxr0 - 2*Rxs0*Rxs0 - 2*Rxt0*Rxt0 
 - 1.
 
// l1 (linearized)
   2*Ryr0*Ryr + 2*Rys0*Rys + 2*Ryt0*Ryt 
 + Ryr0 + Rys0 + Ryt0 
 - 2*Ryr0*Ryr0 - 2*Rys0*Rys0 - 2*Ryt0*Ryt0 
 - 1.
 
// l2 (linearized)
   2*Rzr0*Rzr + 2*Rzs0*Rzs + 2*Rzt0*Rzt 
 + Rzr0 + Rzs0 + Rzt0 
 - 2*Rzr0*Rzr0 - 2*Rzs0*Rzs0 - 2*Rzt0*Rzt0 
 - 1.
 
// l3 (linearized)
   Ryr0*Rxr + Rys0*Rxs + Ryt0*Rxt 
 + Rxr0*Ryr + Rxs0*Rys + Rxt0*Ryt
 - Rxr0*Ryr0 - Rxs0*Rys0 - Rxt0*Ryt0

// l4 (linearized)
   Rzr0*Rxr + Rzs0*Rxs + Rzt0*Rxt 
 + Rxr0*Rzr + Rxs0*Rzs + Rxt0*Rzt		
 - Rxr0*Rzr0 - Rxs0*Rzs0 - Rxt0*Rzt0
 
// l5 (linearized)
   Rzr0*Ryr + Rzs0*Rys + Rzt0*Ryt 
 + Ryr0*Rzr + Rys0*Rzs + Ryt0*Rzt		
 - Ryr0*Rzr0 - Rys0*Rzs0 - Ryt0*Rzt0
 
 /** Note **/
 /**
	The constraints are quadratic (unitary) and bilinear (orthogonality)
	constraints
	
	When we enforce stationarity with \frac{\partial}{\partial l_i}, i\in\{0,5\},
	the constraints must be linearized, otherwise we have quadratic equations to solve.
	
	However, when we look for optimality \frac{\partial}{\partial Rxr} (and \{Rxs, ... Tz\}),
	the quadratic and bilinear constraints become linear in \{Rxr, ... Tz\}, hence we would not
	have to linearize them there
	
	This problem is a typical QCQP: quadratically constrained quadratic program. 
	Is there any nice way to optimize for such a problem other than linearizing?
 **/
 
 /** Todo **/
 /**
	Now we need to collect the terms that are in \{Rxr, ... Tz\} and \{l0,... l5\},
	solve for \{l0, ... l5\} and insert them in the remaining equations.
 **/