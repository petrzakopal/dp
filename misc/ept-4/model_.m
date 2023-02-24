Us=400;% V, sdruzene napeti site
TriangleTime2kHz=[0 0.00025 0.0005];
TriangleData=[0 1 0]; %1.021467 při 230 V Uef
TriangleTime10kHz=[0 5E-5 0.0001];
svmFrequency=10000;


% motor
Pn=12000; % Jmenovity vykon stroje (W)
Un=380; % Jmenovite napeti kotvy (V)
In=22; % Jmenovity proud kotvy (A)
nn=1460; % Jmenovite otacky stroje (min-1)
pp=2; % Pocet polparu
R1=370e-3; % Statorovy odpor vinuti (Ohm)
R2=225e-3; % Rotorovy odpor vinuti (Ohm)
R2_vec_model=225e-3;
L1sigma=2.27e-3; % Rozptylova indukcnost statoroveho vinuti (H)
L2sigma=2.27e-3; % Rozptylova indukcnost rotoroveho vinuti (H)
Lm=82.5e-3; % Magnetizacni indukcnost (H)
L1=L1sigma+Lm; % Statorova indukcnost (H)
L2=L2sigma+Lm; % Rotorova indukcnost (H)
J=0.4; % Moment setrvacnosti hridele (kg*m^(2))
sigma=1-Lm^(2)/(L1*L2); %Koeficient rozptylu
f=50; % Napajeci frekvence



Ufnmax=Un/sqrt(3)*sqrt(2);
omegaN=2*pi*f;
PhiN=acos(0.8);
Inmax=sqrt(2)*In;
psi1n=(sqrt((Ufnmax-R1*Inmax*cos(PhiN))^(2) + (R1*Inmax*sin(PhiN))))/(omegaN);
velkePsi=asin((R1*Inmax*sin(PhiN))/(omegaN*psi1n));
psi1nre=psi1n*cos(velkePsi);
psi1nim=psi1n*sin(velkePsi);
i1nre=Inmax*sin(PhiN);
i1nim=Inmax*cos(PhiN);
psi2nre=(L2)/(Lm)*(psi1nre-sigma*L1*i1nre);
psi2nim=(L2)/(Lm)*(psi1nim-sigma*L1*i1nim);
psi2n=sqrt(psi2nre^2+psi2nim^2);
i1dn=psi2n/Lm;
i1qn=sqrt(Inmax^2-i1dn^2);

kp_current=(sigma*L1)/(2/svmFrequency);
ki_current=1/(((L2^2)/(Lm^2*R2+L2^2*R1))*2/svmFrequency);

kp_field=(L2/R2)/(4*Lm*1/svmFrequency); % nejsem si jistý
ki_field=1/(4*Lm/svmFrequency);

kp_omega=J/(4/svmFrequency*3/2*pp*Lm/L2*abs(psi2nim));
ki_omega=J/(32*(1/svmFrequency)^2*3/2*pp*Lm/L2*abs(psi2nim));
